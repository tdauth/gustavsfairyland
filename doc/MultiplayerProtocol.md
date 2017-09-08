Gustavs M�rchenwelt Netzwerk-Protokoll:
- QtNetwork mit High Level API verwenden
- https://wiki.qt.io/Qt_and_Web_Services

Zur Vereinfachung:
- Maximal 2 Spieler.
- �ber einen zentralen Server.
- Der Host f�ngt mit awaitclip an (immer der gleiche Spieler)
- Maximal 4 Spiele gleichzeitig �ber den Server
- Maximal 15 MiBytes gro�e Clips versenden (Bild + Erz�hlersound + Video)
- Maximal alle 5 Minuten den REST-Aufruf "sendclip" bzw. der Server verarbeitet die REST-Calls aus einer Queue
heraus und immer nur in einem bestimmten Intervall bzw. muss schauen, dass nicht zu viele Calls von einem Client
gequeuet werden!

Der Server ist ein separates Programm, dass auf auf einem Server l�uft, dessen Domain bei jeden Client
eingestellt werden kann:
Domain: wc3lib.org
Port: 80 (HTTP ist meist erlaubt)

Server REST-API:

list
Befehl: GET
Inhalt: Nichts
Antwort: Eine Liste aller Spiele (IDs, Namen, Passwortflag)

create - erzeugt ein Spiel
Befehl: POST
Inhalt: Spielname, Passwort (optional)
Antwort: Gibt die ID des Spiels zur�ck

remove - entfernt ein Spiel
Befehl: DELETE
Inhalt: Spiele-ID, Session-Key

join - man nimmt an einem Spiel teil
Befehl: GET
Inhalt: ID des Spiels, Passwort (wenn notwendig)
Antwort: Bei Erfolg: Session-Key f�r das Spiel. Ansonsten: Fehler

leave - man verl�sst das Spiel
Befehl: GET
Inhalt: Session-Key
Antwort: Best�tigung

awaitstart - warte auf Spielstart
Befehl: GET
Inhalt: ID des Spiels.
Antwort: Sobald es los geht eine Best�tigung oder eine Abbruchnachricht bei Abbruch

Spielmodus-REST-API:
awaitclip
Befehl: GET
Inhalt: Clip-Beschreibung
Antwort: Clip-Datei mit Bild, Video und Erz�hler-Sound

sendclip
Befehl: GET
Inhalt: Clip-Datei mit Bild, Video und Erz�hler-Sound
Antwort: Erfolg oder kein Erfolg