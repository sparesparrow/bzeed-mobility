import { Router } from "express";
import { z } from "zod";
import prisma from "../lib/prisma.js";
import { requireAuth } from "../middleware/auth.js";
const router = Router();
const startSchema = z.object({
    scooterId: z.string().uuid(),
    startLat: z.number().refine((v) => Math.abs(v) <= 90),
    startLng: z.number().refine((v) => Math.abs(v) <= 180),
});
router.post("/start", requireAuth, async (req, res) => {
    const parse = startSchema.safeParse(req.body);
    if (!parse.success)
        return res.status(400).json({ error: parse.error.flatten() });
    const { scooterId, startLat, startLng } = parse.data;
    const { userId } = req;
    const [userActive, scooterActive, scooter] = await Promise.all([
        prisma.ride.findFirst({ where: { userId, endedAt: null } }),
        prisma.ride.findFirst({ where: { scooterId, endedAt: null } }),
        prisma.scooter.findUnique({ where: { id: scooterId } }),
    ]);
    if (!scooter)
        return res.status(404).json({ error: "Scooter not found" });
    if (!scooter.isActive)
        return res.status(400).json({ error: "Scooter not active" });
    if (userActive)
        return res.status(400).json({ error: "User already has an active ride" });
    if (scooterActive)
        return res.status(400).json({ error: "Scooter is in use" });
    const result = await prisma.$transaction(async (tx) => {
        const ride = await tx.ride.create({
            data: { userId, scooterId, startLat, startLng },
        });
        await tx.scooter.update({ where: { id: scooterId }, data: { latitude: startLat, longitude: startLng } });
        return ride;
    });
    return res.status(201).json({ ride: result });
});
const endSchema = z.object({
    rideId: z.string().uuid().optional(),
    endLat: z.number().refine((v) => Math.abs(v) <= 90),
    endLng: z.number().refine((v) => Math.abs(v) <= 180),
});
router.post("/end", requireAuth, async (req, res) => {
    const parse = endSchema.safeParse(req.body);
    if (!parse.success)
        return res.status(400).json({ error: parse.error.flatten() });
    const { rideId, endLat, endLng } = parse.data;
    const { userId } = req;
    const ride = rideId
        ? await prisma.ride.findFirst({ where: { id: rideId, userId, endedAt: null } })
        : await prisma.ride.findFirst({ where: { userId, endedAt: null }, orderBy: { startedAt: "desc" } });
    if (!ride)
        return res.status(404).json({ error: "Active ride not found" });
    const result = await prisma.$transaction(async (tx) => {
        const updated = await tx.ride.update({
            where: { id: ride.id },
            data: { endedAt: new Date(), endLat, endLng },
        });
        await tx.scooter.update({ where: { id: ride.scooterId }, data: { latitude: endLat, longitude: endLng } });
        return updated;
    });
    return res.json({ ride: result });
});
export default router;
//# sourceMappingURL=rides.js.map