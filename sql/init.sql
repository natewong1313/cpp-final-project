
CREATE TABLE IF NOT EXISTS users(
    id TEXT PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    email TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS userTokens(
    token TEXT PRIMARY KEY,
    user_id TEXT NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS servers(
    id TEXT PRIMARY KEY,
    admin_id INTEGER NOT NULL,
    name TEXT NOT NULL,
    FOREIGN KEY (admin_id) REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS messages(
    id TEXT PRIMARY KEY,
    author_id TEXT NOT NULL,
    server_id TEXT NOT NULL,
    content TEXT,
    created_at INTEGER,
    FOREIGN KEY (author_id) REFERENCES users(id),
    FOREIGN KEY (server_id) REFERENCES servers(id)
);