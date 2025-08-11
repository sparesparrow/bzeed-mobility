# Contributing

## Development Setup
- Backend: Node 22, `npm ci`, `npm run dev`
- Android: Android Studio (JDK 17, SDK 34), run `lint` and `test`
- ESP32: PlatformIO (`pip install --user platformio`), `pio run`

## Coding Style
- TypeScript: ESLint + Prettier; run `npm run lint`
- Kotlin: idiomatic Kotlin, MVVM, Hilt for DI, Compose for UI
- C++ (ESP32): keep functions small and clear; prefer `constexpr` and named constants

## Tests
- Backend: Jest unit tests in `backend/test`
- Android: Unit tests in `app/src/test`
- ESP32: Prefer modular code to enable unit tests where possible

## Pull Requests
- Use feature branches; keep PRs focused
- Include tests and update documentation
- Ensure CI passes (backend lint/test/build; android lint/test; esp32 build)