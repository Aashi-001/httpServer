# Simple HTTP 1.0 Web Server in C

This project is a minimal multithreaded HTTP 1.0 web server implemented from scratch in C. It was built as a learning exercise to understand:

- Socket programming
- Basic HTTP protocol handling
- Static file serving
- Multithreading using POSIX threads

## Features

- Accepts and handles concurrent HTTP connections using `pthread`
- Parses HTTP/1.0 `GET` requests (ignores headers)
- Serves files from a `public/` directory
- MIME type handling for common file types (HTML, CSS, JS, PNG, JPG)
- Supports and responds with standard HTTP status codes:
  - `200 OK`
  - `400 Bad Request`
  - `403 Forbidden`
  - `404 Not Found`
  - `501 Not Implemented`

## Build and Run

You can use the provided `Makefile`:

```bash
make build     # Compiles main.c to executable 'main'
make run       # Runs the server (listens on port 8080)
make clean     # Removes the compiled executable
```
## Testing
- `curl http://localhost:8080/index.html`
- `curl http://localhost:8080/test.txt`