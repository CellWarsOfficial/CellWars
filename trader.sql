/*
Example tables for the trader group project.

The definitions can be loaded by running the command:
  psql my_database_name
At the psql prompt you then type:
  \i trader.sql
Ignore any errors from the DROP commands 
(these commands a just removing tables from your database if you have previously set up the database).
  
You can use:   
  pg_dump -f filename database
to save the data and definition of a database in this format. If you want to make complex changes to 
the database you may find it easiest to edit the definitions below and reload this file.
*/


--------------------------------------------------------------------------------------------------
-- First we tidy up earlier versions of the tables if they exist to avoid conflicts.
DROP TABLE Game;
DROP TABLE Players;
DROP TABLE Messages;
DROP TABLE Universe;
DROP TABLE Resources;
DROP TABLE Ships;
DROP TABLE Holds;


--------------------------------------------------------------------------------------------------
-- Next we create the tables for our database.
/*
-- Game table --
Contains the general state of a game. There will be one row for each game in progress.
*/
CREATE TABLE Game (
  GameNumber integer Primary Key, --unique game ID
  NumPlayers integer, --number of players in the game
  GameOver boolean --if the game has finished (initialised to false)
);

/*
-- Players table --
Contains the information about each player. There will be one row for each player.
*/
CREATE TABLE Players (
  PlayerNumber integer Primary Key, --unique player ID
  PlayerName text unique not null , --what the player wants to be called
  GameNumber integer References Game, --which game they are playing
  Credit integer, --how many Imperial Credits the player has
  Playing boolean --if the player is logged on
);

/*
-- Messages table --
Contains the information about each message that is sent from one player to another.
*/
CREATE TABLE Messages (
  MsgNumber integer Primary Key, --unique message ID  
  GameNumber integer References Game, --game message is attached to
  SenderNumber integer References Players, --the sender
  ReceiverNumber integer References Players ,  --the recipient 
  Sent timestamp  NOT NULL, --when message was sent
  Subject text, --the subject line of the message
  Message text --the body of the message
);

/*
-- Universe table --
Contains the game independent information about each star-system in the game. 
For simplicity we assume a 2D star map.
*/
CREATE TABLE Universe (
  StarNumber integer Primary Key, --unique star-system ID
  StarName text NOT NULL, --the name of the star-system
  Type text NOT NULL, --the type of the star-system (Agricultural, Mineral, Industrial or Spice)
  -- the coordinates of the star in light-years with the sun at (0,0,0)
  x double precision NOT NULL,
  y double precision NOT NULL,
  z double precision NOT NULL
);

/*
-- Goods table --
Contains information about the different kinds of goods that can be traded.
*/
CREATE TABLE Resources (
  ResNumber integer Primary Key, --unique resource ID
  Name text NOT NULL , --name of the resource
  Size integer NOT NULL --size of the resource (how much hold space the resource takes up)
);

/*
-- Ships table --
Contains information about each player's ship.
*/
CREATE TABLE Ships (
  ShipNumber integer Primary Key, --unique ship ID
  GameNumber integer, --game it is in
  PlayerNumber integer, --ship owner
  ShipName text NOT NULL , --the ships name
  Capacity integer, --how big the ship's hold is
  TankSize integer, --how big the ship's fuel-tank is
  Load integer, --how many units of cargo the ship is carrying
  Fuel integer, --how many units of fuel are in the ship's fuel tank
  Location integer References Universe --which star-system the ship is in
);

/*
-- Holds table --
Contains information about the resources in each ship's hold.
There will be one entry in this table for each resource type for each ship.
*/
CREATE TABLE Holds (
  ShipNumber integer References Ships, --the ship carrying the resource
  ResNumber integer References Resources , --the type of resource being carried
  Primary Key(ShipNumber,ResNumber), --primary key to ensure no duplication of information
  Count integer NOT NULL --how many units of the resource are in the hold 
);

--------------------------------------------------------------------------------------------------
-- Finally we populate the tables with the basic game state information.

-- Set up the star-systems.
INSERT INTO Universe VALUES(0,'Sol','industrial',0,0,0);
INSERT INTO Universe VALUES(1,'Proxima Centauri','mineral',-2.99638,-2.41866,0.418553);
INSERT INTO Universe VALUES(2,'Alpha Centauri','agricultural',-2.99313,-0.0254794,0.724108); 
INSERT INTO Universe VALUES(3,'Barnard''s Star','industrial',1.39529,-1.10182,-0.954628);
INSERT INTO Universe VALUES(4,'Wolf 359','agricultural',3.71408,4.55907,0.656987);
INSERT INTO Universe VALUES(5,'Lalande 21185','industrial',1.77541,-1.40199,-0.853405);
INSERT INTO Universe VALUES(6,'Sirius','agricultural',-4.90141,-4.29051,0.660915);
INSERT INTO Universe VALUES(7,'UV Ceti','mineral',1.78283,1.68784,0.957889);
INSERT INTO Universe VALUES(8,'Ross 128','mineral',4.82972,-8.27294,0.461779);
INSERT INTO Universe VALUES(9,'Epsilon Eridani','industrial',10.3634,-1.8011,-0.144273);
INSERT INTO Universe VALUES(10,'Arrakis','spice',-9.97803,-2.54813,0.11743);

-- Set up the goods (Imperial Credits, Fuel, Food, Ore, Tools and Spice).
INSERT INTO Resources VALUES(0,'Imperial Credits',0);
INSERT INTO Resources VALUES(1,'Fuel',1);
INSERT INTO Resources VALUES(2,'Food',1);
INSERT INTO Resources VALUES(3,'Ore',1);
INSERT INTO Resources VALUES(4,'Tools',1);
INSERT INTO Resources VALUES(5,'Spice',1);

-- The remaining tables will be populated during the game.
--------------------------------------------------------------------------------------------------





