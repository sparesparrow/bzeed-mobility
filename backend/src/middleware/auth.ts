import type { Request, Response, NextFunction } from "express";
import jwt from "jsonwebtoken";

export type AuthenticatedRequest = Request & { userId: string };

export function requireAuth(req: Request, res: Response, next: NextFunction) {
  const header = req.headers["authorization"];
  if (!header?.startsWith("Bearer ")) {
    return res.status(401).json({ error: "Missing or invalid Authorization header" });
  }
  const token = header.substring("Bearer ".length);
  try {
    const secret = process.env.JWT_SECRET;
    if (!secret) {
      return res.status(500).json({ error: "Server not configured (JWT_SECRET)" });
    }
    const payload = jwt.verify(token, secret) as { userId?: string };
    if (!payload.userId) {
      return res.status(401).json({ error: "Invalid token payload" });
    }
    (req as AuthenticatedRequest).userId = payload.userId;
    return next();
  } catch (err) {
    return res.status(401).json({ error: "Invalid token" });
  }
}
