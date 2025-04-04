-- main schema
CREATE DATABASE IF NOT EXISTS DB;
USE DB;

-- user table
CREATE TABLE IF NOT EXISTS user_tbl(
	user_id VARCHAR(16) NOT NULL PRIMARY KEY,
    user_pw VARCHAR(64) NOT NULL -- SHA256 HEX
);

-- player customize table
CREATE TABLE IF NOT EXISTS player_tbl(
	nickname VARCHAR(8) PRIMARY KEY NOT NULL,
    owner_id VARCHAR(16) NOT NULL, -- pair user_tbl.user_id
    body_type INT,  -- body
    face_type INT,  -- face
    upper_type INT, -- upper
    lower_type INT, -- lower
    shoes_type INT  -- shoes
);