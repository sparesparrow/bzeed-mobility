# Backend (Express + TypeScript)

```mermaid
flowchart LR
  subgraph HTTP Layer
    MW[Middleware] --> RT[Router]
  end
  RT --> CT[Controller]
  CT --> SV[Service]
  SV --> RP[Repository]
  RP --> DB[(PostgreSQL)]
  SV --> Cache[(Redis)]
```

- Endpoints: start with `GET /health`, extend with auth, scooters, rides.
- Config: `.env` with `PORT`, `JWT_SECRET`, `DATABASE_URL`.

## Example request lifecycle
```mermaid
sequenceDiagram
  participant C as Client
  participant H as Express
  participant S as Service
  participant P as PostgreSQL
  C->>H: GET /scooters?near=lat,lng
  H->>S: findNearby(lat,lng,radius)
  S->>P: SELECT ...
  P-->>S: rows
  S-->>H: scooters
  H-->>C: 200 JSON
```
