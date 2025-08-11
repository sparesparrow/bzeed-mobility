<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# Plán implementace kompletní softwarové codebase pro systém sdílených elektrických koloběžek ve Vyškově

## Přehled projektu

Vytvoření komplexního systému pro sdílené elektrické koloběžky vyžaduje koordinovaný vývoj tří hlavních komponent: **ESP32 mikrokontrolér** (pro každou koloběžku), **Android/iOS mobilní aplikace** (pro uživatele) a **PC server backend** (pro správu systému). Tento plán poskytuje strukturovaný přístup k implementaci všech tří komponent.[^1][^2]

## Architektura systému

### Celková architektura

Systém funguje na principu **client-server komunikace** s IoT zařízeními:

- **ESP32 moduly** na koloběžkách komunikují s centrálním serverem
- **Mobilní aplikace** komunikuje se serverem a přímo s ESP32 (přes Bluetooth/Wi-Fi)
- **Server backend** koordinuje všechny operace a ukládá data[^3][^4]


## ESP32 komponenta

### Hardware požadavky

- **ESP32 DevKit** (preferovaně ESP32-S3 nebo ESP32-C3)
- **GPS modul** (NEO-6M nebo NEO-8M)
- **GSM/4G modul** (SIM800L nebo SIM7000)
- **Bluetooth Low Energy** (integrovaný v ESP32)
- **Relay modul** pro ovládání motoru
- **Load cell** nebo snímač pro detekci použití
- **Baterie management systém**[^2][^5]


### ESP32 software architektura

```
src/
├── main.cpp                 # Hlavní program loop
├── config/
│   ├── wifi_config.h       # Wi-Fi konfigurace
│   ├── server_config.h     # Server endpoints
│   └── pins_config.h       # Pin definice
├── modules/
│   ├── gps_module.cpp      # GPS zpracování
│   ├── gsm_module.cpp      # GSM komunikace
│   ├── bluetooth_module.cpp # BLE komunikace
│   ├── lock_control.cpp    # Ovládání zámku
│   └── battery_monitor.cpp # Monitoring baterie
├── communication/
│   ├── http_client.cpp     # HTTP komunikace se serverem
│   ├── mqtt_client.cpp     # MQTT pro real-time data
│   └── ble_server.cpp      # BLE server pro aplikaci
└── utils/
    ├── json_parser.cpp     # JSON zpracování
    └── security.cpp        # Šifrování komunikace
```


### Klíčové funkce ESP32

**Komunikační protokoly:**

- **Wi-Fi/HTTP** pro komunikaci se serverem
- **Bluetooth LE** pro přímou komunikaci s aplikací
- **MQTT** pro real-time updates
- **GSM** jako backup komunikační kanál[^6][^7]

**Hlavní funkcionalita:**

```cpp
// Pseudokód hlavních funkcí
void setup() {
    initWiFi();
    initGPS();
    initBLE();
    connectToServer();
}

void loop() {
    updateGPSLocation();
    checkBatteryLevel();
    handleBLERequests();
    sendDataToServer();
    checkForUpdates();
}
```


### Implementační kroky ESP32

1. **Nastavení vývojového prostředí**
    - Instalace Arduino IDE s ESP32 podporou
    - Přidání board manager URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
    - Instalace potřebných knihoven (WiFi, HTTPClient, ArduinoJson, GPS)[^8][^9]
2. **Základní komunikace**

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

void sendScooterStatus() {
  HTTPClient http;
  http.begin("https://bzeed.eu/api/scooter/status");
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<200> doc;
  doc["scooter_id"] = scooter_id;
  doc["battery"] = getBatteryLevel();
  doc["location"]["lat"] = gps.location.lat();
  doc["location"]["lng"] = gps.location.lng();
  
  String payload;
  serializeJson(doc, payload);
  http.POST(payload);
}
```


## Android aplikace

### Technologický stack

- **Programovací jazyk:** Kotlin
- **Architektura:** MVVM (Model-View-ViewModel)
- **UI Framework:** Jetpack Compose nebo XML layouts
- **Networking:** Retrofit2 + OkHttp
- **Database:** Room (SQLite wrapper)
- **Maps:** Google Maps SDK
- **Bluetooth:** Android Bluetooth LE API[^10][^11]


### Aplikační architektura

```
app/src/main/java/com/vsskoter/
├── ui/
│   ├── activities/
│   │   ├── MainActivity.kt
│   │   ├── MapActivity.kt
│   │   └── RideActivity.kt
│   ├── fragments/
│   │   ├── MapFragment.kt
│   │   ├── ProfileFragment.kt
│   │   └── HistoryFragment.kt
│   └── composables/          # Jetpack Compose UI
├── viewmodels/
│   ├── MapViewModel.kt
│   ├── RideViewModel.kt
│   └── UserViewModel.kt
├── data/
│   ├── repository/
│   │   ├── ScooterRepository.kt
│   │   └── UserRepository.kt
│   ├── network/
│   │   ├── ApiService.kt
│   │   └── NetworkModule.kt
│   ├── database/
│   │   ├── AppDatabase.kt
│   │   ├── entities/
│   │   └── dao/
│   └── models/
├── utils/
│   ├── BluetoothManager.kt
│   ├── LocationManager.kt
│   └── QRCodeScanner.kt
└── di/                      # Dependency Injection (Hilt)
```


### Klíčové funkce aplikace

**Základní features:**

- **Mapa s dostupnými koloběžkami** (Google Maps integration)
- **QR code scanner** pro odemknutí koloběžky
- **Bluetooth komunikace** s ESP32
- **Platební systém** (Stripe nebo místní banka)
- **Historie jízd** a statistiky
- **Uživatelský profil** a nastavení[^12][^13]


### Android implementace

1. **Nastavení projektu**

```kotlin
// build.gradle (Module: app)
dependencies {
    implementation 'com.google.android.gms:play-services-maps:18.1.0'
    implementation 'com.squareup.retrofit2:retrofit:2.9.0'
    implementation 'androidx.room:room-runtime:2.4.3'
    implementation 'com.google.dagger:hilt-android:2.44'
    implementation 'androidx.compose.ui:ui:1.5.0'
}
```

2. **Bluetooth komunikace s ESP32**

```kotlin
class BluetoothScooterManager {
    fun connectToScooter(macAddress: String) {
        // BLE connection implementation
    }
    
    fun unlockScooter(scooterId: String): Boolean {
        // Send unlock command via BLE
        return sendCommand("UNLOCK:$scooterId")
    }
    
    fun getRealTimeData(): ScooterData {
        // Get battery, speed, location from ESP32
    }
}
```

3. **Room database setup**

```kotlin
@Entity
data class Ride(
    @PrimaryKey val id: String,
    val scooterId: String,
    val startTime: Long,
    val endTime: Long?,
    val distance: Float,
    val cost: Double
)

@Dao
interface RideDao {
    @Query("SELECT * FROM ride ORDER BY startTime DESC")
    suspend fun getAllRides(): List<Ride>
}
```


## PC Server Backend

### Technologický stack

- **Runtime:** Node.js 18+
- **Framework:** Express.js
- **Database:** PostgreSQL (primární) + Redis (cache)
- **Authentication:** JWT + OAuth2
- **API:** RESTful API design
- **Real-time:** WebSocket/Socket.io
- **Deployment:** Docker + Linux server[^14][^15]


### Server architektura

```
server/
├── src/
│   ├── controllers/
│   │   ├── scooterController.js
│   │   ├── userController.js
│   │   ├── rideController.js
│   │   └── adminController.js
│   ├── models/
│   │   ├── User.js
│   │   ├── Scooter.js
│   │   └── Ride.js
│   ├── routes/
│   │   ├── api/
│   │   │   ├── scooters.js
│   │   │   ├── users.js
│   │   │   └── rides.js
│   │   └── admin/
│   ├── middleware/
│   │   ├── auth.js
│   │   ├── validation.js
│   │   └── rateLimit.js
│   ├── services/
│   │   ├── paymentService.js
│   │   ├── notificationService.js
│   │   └── geoService.js
│   ├── database/
│   │   ├── connection.js
│   │   └── migrations/
│   └── utils/
│       ├── logger.js
│       └── security.js
├── config/
├── tests/
└── docs/
```


### Database schema

```sql
-- Hlavní tabulky
CREATE TABLE users (
    id UUID PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    phone VARCHAR(20),
    created_at TIMESTAMP DEFAULT NOW(),
    is_active BOOLEAN DEFAULT true
);

CREATE TABLE scooters (
    id UUID PRIMARY KEY,
    unique_code VARCHAR(10) UNIQUE NOT NULL,
    battery_level INTEGER DEFAULT 100,
    latitude DECIMAL(10,8),
    longitude DECIMAL(11,8),
    status ENUM('available', 'in_use', 'maintenance', 'offline'),
    last_update TIMESTAMP DEFAULT NOW()
);

CREATE TABLE rides (
    id UUID PRIMARY KEY,
    user_id UUID REFERENCES users(id),
    scooter_id UUID REFERENCES scooters(id),
    start_time TIMESTAMP DEFAULT NOW(),
    end_time TIMESTAMP,
    start_location POINT,
    end_location POINT,
    distance_km DECIMAL(5,2),
    duration_minutes INTEGER,
    cost_czk DECIMAL(8,2),
    status ENUM('active', 'completed', 'cancelled')
);
```


### REST API endpoints

```javascript
// API routes example
app.get('/api/scooters/nearby', async (req, res) => {
  const { lat, lng, radius = 1000 } = req.query;
  
  const scooters = await Scooter.findNearby(lat, lng, radius);
  res.json(scooters);
});

app.post('/api/rides/start', authenticateToken, async (req, res) => {
  const { scooterId } = req.body;
  const userId = req.user.id;
  
  try {
    const ride = await RideService.startRide(userId, scooterId);
    res.json({ success: true, rideId: ride.id });
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});
```


### Server implementační kroky

1. **Základní Express.js setup**

```javascript
const express = require('express');
const cors = require('cors');
const helmet = require('helmet');

const app = express();

app.use(helmet());
app.use(cors());
app.use(express.json());

// Routes
app.use('/api/scooters', scooterRoutes);
app.use('/api/users', userRoutes);
app.use('/api/rides', rideRoutes);
```

2. **Database connection**

```javascript
const { Pool } = require('pg');

const pool = new Pool({
  user: 'vsskoter_user',
  host: 'localhost',
  database: 'vsskoter_db',
  password: process.env.DB_PASSWORD,
  port: 5432,
});
```


## Integrace komponent

### Komunikační protokoly

**ESP32 ↔ Server:**

- **HTTP POST/GET** pro status updates a příkazy
- **MQTT** pro real-time pozice a telemetrii
- **WebSocket** pro okamžité příkazy[^16]

**App ↔ ESP32:**

- **Bluetooth LE** pro přímou komunikaci (unlock/lock)
- **QR code** pro identifikaci koloběžky[^13]

**App ↔ Server:**

- **RESTful API** pro všechny operace
- **WebSocket** pro real-time updates
- **Push notifications** pro upozornění[^17][^18]


### Security implementace

```javascript
// JWT authentication
const jwt = require('jsonwebtoken');

function authenticateToken(req, res, next) {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[^1];

  if (!token) {
    return res.sendStatus(401);
  }

  jwt.verify(token, process.env.ACCESS_TOKEN_SECRET, (err, user) => {
    if (err) return res.sendStatus(403);
    req.user = user;
    next();
  });
}
```


## Deployment a infrastruktura

### Server deployment na bzeed.eu

1. **Docker containerizace**

```dockerfile
FROM node:18-alpine

WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production

COPY . .
EXPOSE 3000

CMD ["node", "src/server.js"]
```

2. **Nginx reverse proxy**

```nginx
server {
    listen 80;
    server_name api.bzeed.eu;
    
    location /api/ {
        proxy_pass http://localhost:3000/api/;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```


### Monitoring a maintenance

- **Logging:** Winston nebo Bunyan pro strukturované logy
- **Metrics:** Prometheus + Grafana pro monitoring
- **Alerting:** Discord/Slack webhooks pro kritické události
- **Backup:** Automated PostgreSQL backups[^19]


## Časový plán implementace

### Fáze 1 (Týdny 1-3): Základní infrastruktura

- Server backend setup (Express.js + PostgreSQL)
- Základní ESP32 firmware (Wi-Fi komunikace)
- Android projekt setup s základním UI


### Fáze 2 (Týdny 4-6): Core functionality

- REST API implementace
- GPS tracking na ESP32
- Mapa s koloběžkami v aplikaci
- Bluetooth komunikace


### Fáze 3 (Týdny 7-9): Advanced features

- QR code scanning
- Platební systém
- Lock/unlock mechanismus
- Real-time telemetrie


### Fáze 4 (Týdny 10-12): Testing a deployment

- Unit a integration testy
- Security audit
- Performance optimalizace
- Production deployment


## Doporučené knihovny a tools

### ESP32

- **ArduinoJson** - JSON parsing
- **WiFiClientSecure** - HTTPS komunikace
- **BluetoothSerial** - BLE komunikace
- **TinyGPS++** - GPS parsing[^8]


### Android

- **Retrofit2** - HTTP client
- **Room** - SQLite wrapper
- **Hilt** - Dependency injection
- **Jetpack Compose** - Modern UI
- **CameraX** - QR code scanning[^20][^21]


### Server

- **Express.js** - Web framework
- **Sequelize/Prisma** - ORM
- **Jest** - Testing framework
- **Winston** - Logging
- **Redis** - Caching[^22][^23]

Tento plán poskytuje kompletní roadmapu pro implementaci systému sdílených koloběžek se zaměřením na robustnost, bezpečnost a škálovatelnost pro město Vyškov.

<div style="text-align: center">⁂</div>

[^1]: https://ddd.uab.cat/pub/tfg/2019/tfg_182300/FinalReportGEI.pdf

[^2]: https://ijrpr.com/uploads/V6ISSUE4/IJRPR42218.pdf

[^3]: https://www.nickelfox.com/success-stories/hopbike/

[^4]: https://github.com/A-Emile/Lime_Gen3_IoT_Replacement

[^5]: https://www.nabto.com/guide-to-iot-esp-32/

[^6]: https://www.mpythonboard.com/blogs/blogs/esp32-communication-a-complete-overview

[^7]: https://randomnerdtutorials.com/esp32-wireless-communication-protocols/

[^8]: https://randomnerdtutorials.com/getting-started-with-esp32/

[^9]: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

[^10]: https://getstream.io/blog/design-patterns-and-architecture-the-android-developer-roadmap-part-4/

[^11]: https://www.intelivita.com/blog/android-architecture/

[^12]: https://www.echoinnovateit.com/best-scooter-apps/

[^13]: https://www.purrweb.com/blog/how-to-build-a-scooter-sharing-app-a-step-by-step-guide/

[^14]: https://www.geeksforgeeks.org/node-js-web-application-architecture/

[^15]: https://www.w3schools.com/nodejs/nodejs_architecture.asp

[^16]: https://www.ijsdr.org/papers/IJSDR2305015.pdf

[^17]: https://daily.dev/blog/restful-api-design-best-practices-guide-2024

[^18]: https://stackoverflow.blog/2020/03/02/best-practices-for-rest-api-design/

[^19]: https://blog.logrocket.com/node-js-project-architecture-best-practices/

[^20]: https://clouddevs.com/android/sqlite-database/

[^21]: https://www.geeksforgeeks.org/android/how-to-create-and-add-data-to-sqlite-database-in-android/

[^22]: https://dev.to/kafeel_ahmad/how-to-structure-your-backend-code-in-nodejs-expressjs-2e07

[^23]: https://github.com/fifocode/nodejs-backend-architecture-typescript

[^24]: https://en.wikipedia.org/wiki/Scooter-sharing_system

[^25]: https://ijemr.vandanapublications.com/index.php/j/article/view/1253

[^26]: https://www.altoroslabs.com/portfolio/portfolio_a-mobile-app-for-renting-electric

[^27]: https://ieeexplore.ieee.org/document/10421849

[^28]: https://whitelabelfox.com/e-scooter-app-development/

[^29]: https://www.sciencedirect.com/science/article/pii/S2590198223001355

[^30]: https://rexsoftinc.com/e-scooter-app-development

[^31]: https://ieeexplore.ieee.org/document/9213514/

[^32]: https://www.hackster.io/512014/iot-enabled-electric-scooter-using-hexabitz-module-f1a436

[^33]: https://www.techgropse.com/e-scooter-app-development

[^34]: https://www.sciencedirect.com/science/article/abs/pii/S0968090X21004988

[^35]: https://www.scribd.com/document/829784347/Xiaomi-M365-scooter-dashboard-with-ESP32-and-1

[^36]: https://appdesign.dev/en/mobility-app-project/

[^37]: https://www.omnismartiot.com/News/How-to-track-scooter-with-IoT-GPS-tracker.html

[^38]: https://www.suntechapps.com/services/electric-scooter-app-development-company

[^39]: https://oyelabs.com/e-scooter-app-development-company/

[^40]: https://ijaem.net/issue_dcp/A%20Review%20paper%20on%20Smart%20Helmet%20for%20Rental%20Vehicles.pdf

[^41]: https://github.com/Haim02/share-and-ride

[^42]: https://www.slideshare.net/slideshow/software-engineering-projectbikes-and-scooters-rental-system/238327427

[^43]: https://www.mobindustry.net/blog/how-to-set-up-the-backend-and-admin-panel-for-a-scooter-sharing-startup-top-web-technologies/

[^44]: https://github.com/Xiangs18/Bike_Rental_System

[^45]: https://scootapi.com/en/

[^46]: https://rexsoftinc.com/scooter-sharing-software

[^47]: https://ideausher.com/blog/e-scooter-sharing-application-features-benefits-and-development-cost/

[^48]: https://www.reddit.com/r/embedded/comments/1k7bm9v/what_do_you_actually_use_at_work/

[^49]: https://oyelabs.com/e-scooter-app-development-guide/

[^50]: https://scootapi.com/en/blog/custom-ride-sharing-app-vs-ready-made-system-solution/

[^51]: https://patents.google.com/patent/US20200242471A1/en

[^52]: https://www.nimbleappgenie.com/blogs/how-to-develop-an-escooter-app/

[^53]: https://stormotion.io/blog/e-scooter-app-development/

[^54]: https://news.ycombinator.com/item?id=37016842

[^55]: https://github.com/MihaiAnton/CityWay

[^56]: https://www.techrm.com/practical-guide-to-esp32-communication-protocols/

[^57]: https://www.geeksforgeeks.org/android/android-architecture-patterns/

[^58]: https://developer.android.com/topic/architecture

[^59]: https://docs.arduino.cc/tutorials/nano-esp32/esp-now

[^60]: https://developer.android.com/topic/modularization/patterns

[^61]: https://www.einfochips.com/blog/mastering-esp32-wi-fi-and-mqtt-a-beginners-guide-to-building-smart-iot-devices/

[^62]: https://radixweb.com/nodejs-architecture

[^63]: https://www.geeksforgeeks.org/system-design/design-patterns-for-mobile-development/

[^64]: https://www.espressif.com/en/solutions/low-power-solutions/esp-now

[^65]: https://webandcrafts.com/blog/nodejs-architecture

[^66]: https://zuplo.com/learning-center/common-pitfalls-in-restful-api-design

[^67]: https://www.youtube.com/watch?v=ikBlhX-erSw

[^68]: https://www.digitalocean.com/community/tutorials/android-sqlite-database-example-tutorial

[^69]: https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html

[^70]: https://developer.android.com/training/data-storage/sqlite

[^71]: https://www.getambassador.io/blog/7-rest-api-design-best-practices

[^72]: https://www.upesy.com/blogs/tutorials/install-esp32-on-arduino-ide-complete-guide

[^73]: https://www.topcoder.com/thrive/articles/sqlite-database-in-android

[^74]: https://learn.microsoft.com/en-us/azure/architecture/best-practices/api-design

[^75]: https://www.youtube.com/watch?v=CD8VJl27n94

[^76]: https://developer.android.com/topic/performance/sqlite-performance-best-practices

[^77]: https://swagger.io/resources/articles/best-practices-in-api-design/

[^78]: https://www.instructables.com/ESP32-With-Arduino-IDE/

[^79]: https://www.tutorialspoint.com/android/android_sqlite_database.htm

