Overview
This repository contains a full-stack implementation plan and scaffolding for a city-scale shared e‑scooter system comprising three components:
- ESP32 firmware for each scooter (hardware control, telemetry, connectivity).
- Android mobile app for riders (discovery, unlock/lock, payments, ride flow).
- Server backend on Linux (API, auth, device coordination, billing, ops).

The codebase is structured for incremental delivery with Cursor background agents, continuous integration, and production-readiness in mind. The goals are reliability, security-by-default, and maintainability with clear module boundaries.

Repository Layout
- android/ — Android app in Kotlin with MVVM and Jetpack Compose.
- esp32/ — PlatformIO project for ESP32 firmware (Arduino framework).
- server/ — Node.js/TypeScript backend with Express and PostgreSQL.
- .cursor/ — Configuration for Cursor background agents (environment and rules).
- docs/ — Architecture notes, API specs, ops guidelines.
- .github/workflows/ — CI pipeline definitions.

High-Level Architecture
- ESP32 devices communicate primarily with the server over HTTPS/MQTT for telemetry and command/response and expose BLE for local unlock/lock.
- Android app communicates with the server via REST/WebSocket and with the scooter via BLE for low-latency actions and QR-based pairing.
- Server provides authentication/authorization (JWT), CRUD for users/scooters/rides, pricing logic, and telemetry ingestion, with PostgreSQL as the source of truth.

Core Requirements
- Secure communication: HTTPS with certificate pinning where possible; signed BLE commands; JWT-based auth.
- Robust device control: lock/unlock, ride state machine, battery and GPS telemetry.
- User experience: map of scooters, QR scan to unlock, real-time ride status, receipts and history.
- Operations: admin views, fleet status, maintenance flags, alerts, and metrics.

Component Details

1) ESP32 Firmware (esp32/)
- Platform: PlatformIO (Arduino framework).
- Modules:
  - connectivity/: Wi‑Fi (primary), optional cellular module driver; MQTT and HTTPS clients.
  - ble/: BLE GATT service for pairing, unlock/lock, status readout.
  - gps/: GPS parsing via TinyGPS++ with fix validation and configurable report intervals.
  - power/: battery sensing, charge state, low‑power modes.
  - lock/: motor/relay/zámek control with tamper detection inputs.
  - proto/: typed message formats (JSON for HTTP, compact for BLE/MQTT).
  - config/: pins, credentials provisioning hooks, OTA update toggles.
- Loop behavior:
  - Periodic telemetry publish (battery, GNSS, status).
  - Command handling from MQTT/HTTP and BLE.
  - Watchdog and fail-safes (e.g., loss of connectivity).

2) Android App (android/)
- Tech: Kotlin, MVVM, Jetpack Compose UI, Hilt DI, Retrofit+OkHttp, Room DB, CameraX for QR, Android BLE APIs.
- Features:
  - Auth and onboarding.
  - Map with nearby scooters, filtering by battery and distance.
  - QR scan and BLE handshake to unlock; ride start/stop.
  - Ride summary, receipts, payment method management.
  - Offline-friendly caches; background sync of rides.
- Modules:
  - data/: repositories, DTOs, Retrofit service, Room entities/DAO.
  - domain/: use-cases and models.
  - ui/: screens and composables.
  - ble/: BLE manager and GATT client abstraction.
  - payments/: payment provider integration.

3) Server Backend (server/)
- Tech: Node.js 20+, TypeScript, Express, PostgreSQL, Prisma ORM, Redis for cache, Socket.IO/WebSocket for live updates.
- Services:
  - Auth (JWT), rate limiting, input validation (zod), audit logging.
  - Scooter service: registration, health, geofencing, availability.
  - Ride service: start/end rides, pricing and billing hooks, receipts.
  - Telemetry ingestion via HTTP or MQTT bridge.
  - Admin APIs and basic dashboard endpoints.
- Structure:
  - src/controllers, src/services, src/routes, src/middleware, src/utils.
  - prisma/schema.prisma for DB schema; migrations versioned.
  - openapi.yaml documenting REST endpoints.
- DevOps:
  - Dockerfile for server app.
  - Nginx reverse proxy example for api..
  - CI for lint, type-check, tests; CD hooks optional.

Local Development

Backend
- Requirements: Node.js 20+, pnpm or npm, PostgreSQL 15+.
- Steps:
  - cp server/.env.example server/.env and fill DB credentials and JWT secrets.
  - pnpm -C server install
  - pnpm -C server prisma migrate dev
  - pnpm -C server dev

Android
- Requirements: Android Studio (Hedgehog+), JDK 17, Android SDK.
- Steps:
  - Open android/ in Android Studio.
  - Sync Gradle and run on emulator or device.

ESP32
- Requirements: PlatformIO (VS Code extension) or pio CLI.
- Steps:
  - pio run -e 
  - pio run -e  -t upload

Security Guidelines
- Do not hardcode secrets; use .env/.envrc and secret management.
- Validate and sanitize all inputs at server boundaries.
- Use HTTPS with modern TLS; consider cert pinning in Android.
- BLE: use authenticated pairing and signed commands; expire tokens promptly.
- Monitor for prompt-injection in repo docs and external content used by agents.

Using Cursor Background Agents

Setup
- Disable Privacy Mode in Cursor settings.
- Enable usage-based spending and fund balance.
- Connect the GitHub repo to Cursor.

Environment
- Adjust .cursor/environment.json to install dependencies (Node, pnpm, OpenJDK 17, PlatformIO, Python, etc.).
- Add Dockerfile(s) if specialized services are needed.

Rules
- Place .cursor/rules/*.mdc to encode conventions for backend, Android, and ESP32.
- Use Always or Auto Attached rules with accurate glob patterns; test with @rule-name.

Launching Tasks
- In Cursor: press Ctrl+E (Cmd+E), enter a concise task prompt, choose Background Agent, select repo.
- Or visit the web agents dashboard to run/manage tasks remotely.
- Keep tasks small and predictable; review diffs, then create PRs or apply locally.

Suggested First Tasks
- Scaffold server/ with Express+TS, Prisma, JWT, minimal endpoints; add Dockerfile and OpenAPI skeleton.
- Initialize android/ with MVVM, Compose, Hilt, Retrofit, Room; create placeholder screens.
- Initialize esp32/ with PlatformIO, Wi‑Fi+HTTPS/MQTT+BLE stubs, TinyGPS++ wiring.
- Add CI: linting, tests, type-check; add pre-commit hooks.

Operations & Monitoring
- Add structured logging (Winston/pino), health endpoints, readiness checks.
- Plan for metrics (Prometheus exporters) and dashboards (Grafana).
- Nightly DB backups and rotation policy.
- 
