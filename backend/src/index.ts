import "dotenv/config";
import express from "express";
import cors from "cors";
import morgan from "morgan";
import prisma from "./lib/prisma.js";
import authRoutes from "./routes/auth.js";
import scootersRoutes from "./routes/scooters.js";
import ridesRoutes from "./routes/rides.js";

const app = express();

app.use(cors());
app.use(express.json());
app.use(morgan("dev"));

app.get("/healthz", async (_req, res) => {
  try {
    await prisma.$queryRaw`SELECT 1`;
    res.json({ status: "ok" });
  } catch (e) {
    res.status(500).json({ status: "error" });
  }
});

app.use("/api/auth", authRoutes);
app.use("/api/scooters", scootersRoutes);
app.use("/api/rides", ridesRoutes);

const port = Number(process.env.PORT || 3000);
app.listen(port, () => {
  console.log(`Server listening on :${port}`);
});
