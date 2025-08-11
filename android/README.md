# Android (Kotlin + Jetpack Compose)

## Overview
- MVVM with Hilt DI
- Compose UI with Navigation
- Retrofit client for backend API
- BLE service stub
- QR scanner scaffolding using CameraX

## Modules
- `app` module only for now

## Running
- Open `android` in Android Studio (JDK 17; SDK 34)
- Select a device/emulator and Run
- Lint: from IDE or CI; Tests: from IDE or CI

## Permissions
- Camera (QR), Bluetooth (connect/scan), Location (BLE scan on Android 12+)
- Runtime permissions must be requested by screens before using camera/ble

## Navigation
- `Map` → `Ride` → `Profile` via `AppNavGraph`

## DI
- Hilt `@HiltAndroidApp` application class and modules in `di/`