import { Router } from "express";
import { z } from "zod";
import bcrypt from "bcryptjs";
import jwt from "jsonwebtoken";
import prisma from "../lib/prisma.js";
const router = Router();
const registerSchema = z.object({
    email: z.string().email(),
    password: z.string().min(6),
    name: z.string().min(1).optional(),
});
router.post("/register", async (req, res) => {
    const parse = registerSchema.safeParse(req.body);
    if (!parse.success)
        return res.status(400).json({ error: parse.error.flatten() });
    const { email, password, name } = parse.data;
    const existing = await prisma.user.findUnique({ where: { email } });
    if (existing)
        return res.status(409).json({ error: "Email already registered" });
    const hashed = await bcrypt.hash(password, 10);
    const user = await prisma.user.create({ data: { email, password: hashed, name } });
    const secret = process.env.JWT_SECRET;
    const token = jwt.sign({ userId: user.id }, secret, { expiresIn: "7d" });
    return res.status(201).json({ token, user: { id: user.id, email: user.email, name: user.name } });
});
const loginSchema = z.object({
    email: z.string().email(),
    password: z.string().min(1),
});
router.post("/login", async (req, res) => {
    const parse = loginSchema.safeParse(req.body);
    if (!parse.success)
        return res.status(400).json({ error: parse.error.flatten() });
    const { email, password } = parse.data;
    const user = await prisma.user.findUnique({ where: { email } });
    if (!user)
        return res.status(401).json({ error: "Invalid credentials" });
    const ok = await bcrypt.compare(password, user.password);
    if (!ok)
        return res.status(401).json({ error: "Invalid credentials" });
    const secret = process.env.JWT_SECRET;
    const token = jwt.sign({ userId: user.id }, secret, { expiresIn: "7d" });
    return res.json({ token, user: { id: user.id, email: user.email, name: user.name } });
});
export default router;
//# sourceMappingURL=auth.js.map