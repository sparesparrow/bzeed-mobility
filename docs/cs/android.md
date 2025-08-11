# Android aplikace (Kotlin + Compose)

```mermaid
flowchart TB
  UI[Compose UI] --> VM[ViewModel]
  VM --> Repo[Repository]
  Repo --> API[Retrofit API]
  Repo --> BLE[BLE Manager]
  Repo --> DB[(Room)]
  API --> VM
  BLE --> VM
  DB --> VM
```

- Navigace: Mapa → Jízda → Profil
- Oprávnění: Kamera, BLE, Poloha
- DI: Hilt moduly v `di/`
