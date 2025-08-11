import { Router } from "express";
import { z } from "zod";
import prisma from "../lib/prisma.js";

const router = Router();

const nearbyQuerySchema = z.object({
  lat: z.coerce.number().refine((v) => Math.abs(v) <= 90, "lat must be in [-90,90]"),
  lng: z.coerce.number().refine((v) => Math.abs(v) <= 180, "lng must be in [-180,180]"),
  radius: z.coerce.number().min(10).max(20000).default(1000), // meters
  limit: z.coerce.number().min(1).max(100).default(20),
});

function haversineMeters(lat1: number, lon1: number, lat2: number, lon2: number): number {
  const R = 6371000;
  const dLat = ((lat2 - lat1) * Math.PI) / 180;
  const dLon = ((lon2 - lon1) * Math.PI) / 180;
  const a =
    Math.sin(dLat / 2) * Math.sin(dLat / 2) +
    Math.cos((lat1 * Math.PI) / 180) *
      Math.cos((lat2 * Math.PI) / 180) *
      Math.sin(dLon / 2) * Math.sin(dLon / 2);
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  return R * c;
}

router.get("/nearby", async (req, res) => {
  const parse = nearbyQuerySchema.safeParse(req.query);
  if (!parse.success) return res.status(400).json({ error: parse.error.flatten() });
  const { lat, lng, radius, limit } = parse.data;

  const latDelta = radius / 111_000; // deg per meter approx
  const lngDelta = radius / (111_000 * Math.cos((lat * Math.PI) / 180));

  const candidates = await prisma.scooter.findMany({
    where: {
      isActive: true,
      latitude: { gte: lat - latDelta, lte: lat + latDelta },
      longitude: { gte: lng - lngDelta, lte: lng + lngDelta },
      rides: { none: { endedAt: null } },
    },
    take: Math.min(limit * 4, 200),
  });

  const withDistance = candidates
    .map((s) => ({
      scooter: s,
      distance: haversineMeters(lat, lng, s.latitude, s.longitude),
    }))
    .filter((x) => x.distance <= radius)
    .sort((a, b) => a.distance - b.distance)
    .slice(0, limit)
    .map(({ scooter, distance }) => ({
      id: scooter.id,
      identifier: scooter.identifier,
      latitude: scooter.latitude,
      longitude: scooter.longitude,
      battery: scooter.battery,
      distance,
    }));

  return res.json({ scooters: withDistance });
});

export default router;
