# Security Policy

- Do not commit secrets (API keys, JWT secrets, DB credentials). Use environment variables and CI/CD secret stores.
- Rotate credentials regularly. Prefer short-lived tokens for CI.
- Backend: require `JWT_SECRET` and `DATABASE_URL` via environment. Fail fast if missing.
- Android: never hardcode API keys or secrets in code or resources. Use remote config where applicable.
- ESP32: avoid embedding production credentials in firmware; prefer device provisioning flows.

## Reporting a Vulnerability
Please open a private security advisory or contact the maintainers. Do not open public issues for vulnerabilities.