# Monorepo: Backend (Node+TS+Express), Android (Kotlin+Compose), ESP32 (PlatformIO)

This repository is a multi-project monorepo containing:

- `backend`: Node.js + TypeScript + Express API
- `android`: Android app written in Kotlin with Jetpack Compose
- `esp32`: PlatformIO project targeting ESP32 (Arduino framework)

## Quick start

- Backend
  - Install deps: `cd backend && npm ci`
  - Run dev: `npm run dev`
  - Test: `npm test`
  - Lint: `npm run lint`
  - Build: `npm run build`

- Android
  - Open `android` in Android Studio (Arctic Fox or newer)
  - Select a device/emulator and run. Gradle will sync automatically.
  - Lint/Test from Gradle tool window or CLI.

- ESP32
  - Requires PlatformIO (`pip install platformio`)
  - Build: `cd esp32 && pio run`
  - Upload: `pio run -t upload`

## CI

GitHub Actions run per-package lint/test:
- Backend: installs Node, lints with ESLint, runs Jest tests, and builds
- Android: sets up Java and Android SDK, runs Gradle `lint` and `test`
- ESP32: installs PlatformIO and builds firmware

## Structure

```
backend/
android/
esp32/
.github/workflows/
```

 
