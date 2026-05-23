# Browser Game Server

A backend server for a browser-based game where players control a dog, explore a map, collect items, and deliver them back to base.

This project is designed as the server-side foundation for an online game experience. It handles game state, map interaction, player actions, persistence, and database-backed storage.

## Project Overview

The game concept is simple and approachable:

- The player controls a dog in the browser
- The dog moves around a game map
- Items appear on the map and can be collected
- Collected items are brought back to the base
- Game progress and server state are persisted

The server is containerized with Docker, making it easy to run locally or demonstrate without manual dependency setup.

## Technology

- C++ game server
- PostgreSQL database
- Docker and Docker Compose for local deployment
- Persistent Docker volumes for database data and saved game state

## What The Docker Setup Includes

The project starts two services:

- `game-server`: runs the game backend and exposes it on port `8080`
- `postgres`: stores game data in PostgreSQL

Docker volumes are used to keep database data and saved game state between restarts.

## How To Start

Make sure Docker Desktop is running.

From the project root, run:

```sh
docker compose up --build
```

After the containers start, open:

```text
http://localhost:8080
```

## How To Stop

Stop the running containers:

```sh
docker compose down
```

To stop the project and remove saved Docker volumes, including database data and saved game state, run:

```sh
docker compose down -v
```

## Docker Troubleshooting

If Docker returns an error such as `Bad Gateway`, `EOF`, or `cannot find //./pipe/docker_engine`, Docker Desktop may not be running correctly or the wrong Docker context may be selected.

Start or restart Docker Desktop, wait until it reports that Docker is running, then run:

```sh
docker context use desktop-linux
docker info
docker compose up --build
```

If Docker API environment variables were previously set in the shell, unset them before retrying:

```sh
unset DOCKER_HOST DOCKER_API_VERSION
```
