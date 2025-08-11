# syntax=docker/dockerfile:1

# Build stage
FROM node:22-alpine AS build
WORKDIR /app

# Install dependencies
COPY package*.json ./
RUN npm ci

# Build sources
COPY tsconfig.json ./
COPY src ./src
RUN npm run build

# Production runtime
FROM node:22-alpine AS runtime
WORKDIR /app
ENV NODE_ENV=production

# Copy only production files
COPY package*.json ./
RUN npm ci --omit=dev
COPY --from=build /app/dist ./dist

EXPOSE 3000
CMD ["node", "dist/index.js"]