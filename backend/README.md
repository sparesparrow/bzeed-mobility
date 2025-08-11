# Backend (Node + TypeScript + Express)

## Commands
- Install: `npm ci`
- Dev: `npm run dev`
- Lint: `npm run lint`
- Test: `npm test`
- Build: `npm run build`
- Start (built): `npm start`

## Configuration
Copy `.env.example` to `.env` and fill values:
- `PORT` (default 3000)
- `JWT_SECRET` (required for auth when implemented)
- `DATABASE_URL` (PostgreSQL connection string if used)

## Docker
- Build: `docker build -t backend:dev .`
- Run: `docker run --rm -p 3000:3000 --env-file .env backend:dev`

## Endpoints
- `GET /health` → `{ "status": "ok" }`

## CI
- GitHub Actions: Node 22, `npm ci`, `npm run lint`, `npm test -- --ci`, `npm run build`