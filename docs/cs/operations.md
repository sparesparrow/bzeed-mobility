# Provoz a CI/CD

```mermaid
flowchart LR
  Dev[Vývojář] --> PR[Pull Request]
  PR --> CI[GitHub Actions]
  CI --> Lint[Lint/Test/Build]
  Lint -- ok --> Review[Kód review]
  Review --> Merge
  Merge --> Docker[Sestavení Docker image]
  Docker --> Deploy[Nasazení na server]
  Deploy --> Monitor[Logy & Metriky]
```

- Prostředí: dev/staging/prod
- Sekrety přes CI nebo orchestrátor, nikdy ve VCS
