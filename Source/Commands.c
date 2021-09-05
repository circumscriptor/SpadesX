#include "Packets.h"
#include "Protocol.h"
#include "Structs.h"
#include "Commands.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

float max(float num1, float num2)
{
    return (num1 > num2) ? num1 : num2;
}

void handleCommands(Server* server, uint8 player, char* message)
{
    Player srvPlayer = server->player[player];
    char   command[30];
    sscanf(message, "%s", command);
    uint8 strLenght = strlen(command);
    for (uint8 i = 1; i < strLenght; ++i) {
        message[i] = command[i] = tolower(command[i]);
    }
    if (strcmp(command, "/kill") == 0) {
        int id = 0;
        if (sscanf(message, "%s #%d", command, &id) == 1) {
            sendKillPacket(server, player, player, 0, 5);
        } else {
            if (server->player[id].state == STATE_READY &&
                (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
            {
                sendKillPacket(server, id, id, 0, 5);
            } else {
                sendServerNotice(server, player, "Player does not exist or isnt spawned yet");
            }
        }
    } else if (strcmp(command, "/tk") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID;
        if (sscanf(message, "%s #%d", command, &ID) == 1) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                if (server->globalAK == 1) {
                    server->globalAK = 0;
                    broadcastServerNotice(server, "Killing has been disabled");
                } else if (server->globalAK == 0) {
                    server->globalAK = 1;
                    broadcastServerNotice(server, "Killing has been enabled");
                }
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            char broadcastDis[100] = "Killing has been disabled for player: ";
            char broadcastEna[100] = "Killing has been enabled for player: ";
            strcat(broadcastDis, server->player[ID].name);
            strcat(broadcastEna, server->player[ID].name);
            if (server->player[ID].allowKilling == 1) {
                server->player[ID].allowKilling = 0;
                broadcastServerNotice(server, broadcastDis);
            } else if (server->player[ID].allowKilling == 0) {
                server->player[ID].allowKilling = 1;
                broadcastServerNotice(server, broadcastEna);
            }
        }
    } else if (strcmp(command, "/ttk") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID = 33;
        if (sscanf(message, "%s #%d", command, &ID) == 2) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                char broadcastDis[100] = "Team Killing has been disabled for player: ";
                char broadcastEna[100] = "Team Killing has been enabled for player: ";
                strcat(broadcastDis, server->player[ID].name);
                strcat(broadcastEna, server->player[ID].name);
                if (server->player[ID].allowTeamKilling) {
                    server->player[ID].allowTeamKilling = 0;
                    broadcastServerNotice(server, broadcastDis);
                } else {
                    server->player[ID].allowTeamKilling = 1;
                    broadcastServerNotice(server, broadcastEna);
                }
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            sendServerNotice(server, player, "You did not enter ID or entered incorrect argument");
        }
    } else if (strcmp(command, "/tb") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID;
        if (sscanf(message, "%s #%d", command, &ID) == 1) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                if (server->globalAB == 1) {
                    server->globalAB = 0;
                    broadcastServerNotice(server, "Building has been disabled");
                } else if (server->globalAB == 0) {
                    server->globalAB = 1;
                    broadcastServerNotice(server, "Building has been enabled");
                }
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            char broadcastDis[100] = "Building has been disabled for player: ";
            char broadcastEna[100] = "Building has been enabled for player: ";
            strcat(broadcastDis, server->player[ID].name);
            strcat(broadcastEna, server->player[ID].name);
            if (server->player[ID].canBuild == 1) {
                server->player[ID].canBuild = 0;
                broadcastServerNotice(server, broadcastDis);
            } else if (server->player[ID].canBuild == 0) {
                server->player[ID].canBuild = 1;
                broadcastServerNotice(server, broadcastEna);
            }
        }
    } else if (strcmp(command, "/kick") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID = 33;
        if (sscanf(message, "%s #%d", command, &ID) == 2) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                char sendingMessage[100];
                strcpy(sendingMessage, server->player[ID].name);
                strcat(sendingMessage, " has been kicked");
                enet_peer_disconnect(server->player[ID].peer, REASON_KICKED);
                broadcastServerNotice(server, sendingMessage);
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            sendServerNotice(server, player, "You did not enter ID or entered incorrect argument");
        }
    } else if (strcmp(command, "/mute") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID = 33;
        if (sscanf(message, "%s #%d", command, &ID) == 2) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                char sendingMessage[100];
                strcpy(sendingMessage, server->player[ID].name);
                if (server->player[ID].muted) {
                    server->player[ID].muted = 0;
                    strcat(sendingMessage, " has been unmuted");
                } else {
                    server->player[ID].muted = 1;
                    strcat(sendingMessage, " has been muted");
                }
                broadcastServerNotice(server, sendingMessage);
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            sendServerNotice(server, player, "You did not enter ID or entered incorrect argument");
        }
    } else if (strcmp(command, "/ups") == 0) {
        float ups = 0;
        char  upsString[4];
        sscanf(message, "%s %f", command, &ups);
        sscanf(message, "%s %s", command, upsString);
        if (ups >= 1 && ups <= 300) {
            server->player[player].ups = ups;
            char fullString[32]        = "UPS changed to ";
            strcat(fullString, upsString);
            strcat(fullString, " succesufully");
            sendServerNotice(server, player, fullString);
        } else {
            sendServerNotice(server, player, "Changing UPS failed. Please select value between 1 and 300");
        }
    } else if (strcmp(command, "/pban") == 0 &&
               (srvPlayer.isManager || srvPlayer.isAdmin || srvPlayer.isMod || srvPlayer.isGuard))
    {
        int ID = 33;
        if (sscanf(message, "%s #%d", command, &ID) == 2) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                char sendingMessage[100];
                strcpy(sendingMessage, server->player[ID].name);
                strcat(sendingMessage, " has been banned");
                FILE* fp;
                fp = fopen("BanList.txt", "a");
                if (fp == NULL) {
                    sendServerNotice(server, player, "Player could not be banned. File failed to open");
                    return;
                }
                fprintf(fp, "%d %s\n", server->player[ID].peer->address.host, server->player[ID].name);
                fclose(fp);
                enet_peer_disconnect(server->player[ID].peer, REASON_BANNED);
                broadcastServerNotice(server, sendingMessage);
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            sendServerNotice(server, player, "You did not enter ID or entered incorrect argument");
        }
    } else if (strcmp(command, "/login") == 0) {
        if (!server->player[player].isManager && !server->player[player].isAdmin && !server->player[player].isMod &&
            !server->player[player].isGuard && !server->player[player].isTrusted)
        {
            char level[32];
            char password[64];
            if (sscanf(message, "%s %s %s", command, level, password) == 3) {
                uint8 levelLen = strlen(level);
                for (uint8 j = 0; j < levelLen; ++j) {
                    level[j] = tolower(level[j]);
                }
                if (strcmp(level, "manager") == 0) {
                    if (strcmp(password, server->managerPasswd) == 0) {
                        server->player[player].isManager = 1;
                        sendServerNotice(server, player, "You have logged in as manager");
                    } else {
                        sendServerNotice(server, player, "Incorrect password");
                    }
                } else if (strcmp(level, "admin") == 0) {
                    if (strcmp(password, server->adminPasswd) == 0) {
                        server->player[player].isAdmin = 1;
                        sendServerNotice(server, player, "You have logged in as admin");
                    } else {
                        sendServerNotice(server, player, "Incorrect password");
                    }
                } else if (strcmp(level, "mod") == 0 || strcmp(level, "moderator") == 0) {
                    if (strcmp(password, server->modPasswd) == 0) {
                        server->player[player].isMod = 1;
                        sendServerNotice(server, player, "You have logged in as moderator");
                    } else {
                        sendServerNotice(server, player, "Incorrect password");
                    }
                } else if (strcmp(level, "guard") == 0) {
                    if (strcmp(password, server->guardPasswd) == 0) {
                        server->player[player].isGuard = 1;
                        sendServerNotice(server, player, "You have logged in as guard");
                    } else {
                        sendServerNotice(server, player, "Incorrect password");
                    }
                } else if (strcmp(level, "trusted") == 0) {
                    if (strcmp(password, server->trustedPasswd) == 0) {
                        server->player[player].isTrusted = 1;
                        sendServerNotice(server, player, "You have logged in as trsuted");
                    } else {
                        sendServerNotice(server, player, "Incorrect password");
                    }
                } else {
                    sendServerNotice(server, player, "Incorrect role");
                }
            } else {
                sendServerNotice(server, player, "Incorrect number of arguments to login");
            }
        } else {
            sendServerNotice(server, player, "You are already logged in");
        }
    } else if (strcmp(command, "/ratio") == 0) {
        int ID = 33;
        if (sscanf(message, "%s #%d", command, &ID) == 2) {
            if (ID >= 0 && ID < 31 && isPastJoinScreen(server, ID)) {
                char sendingMessage[100];
                char ratioInString[12];
                char killsInString[4];
                char deathsInString[4];
                gcvt(((float) server->player[ID].kills / max(1, (float) server->player[ID].deaths)), 7, ratioInString);
                sprintf(killsInString, "%d", server->player[ID].kills);
                sprintf(deathsInString, "%d", server->player[ID].deaths);
                strcpy(sendingMessage, server->player[ID].name);
                strcat(sendingMessage, " has kill to death ratio of: ");
                strcat(sendingMessage, ratioInString);
                strcat(sendingMessage, " (Kills: ");
                strcat(sendingMessage, killsInString);
                strcat(sendingMessage, ", Deaths: ");
                strcat(sendingMessage, deathsInString);
                strcat(sendingMessage, ")");
                sendServerNotice(server, player, sendingMessage);
            } else {
                sendServerNotice(server, player, "ID not in range or player doesnt exist");
            }
        } else {
            char sendingMessage[100];
            char ratioInString[12];
            char killsInString[4];
            char deathsInString[4];
            gcvt(
            ((float) server->player[player].kills / max(1, (float) server->player[player].deaths)), 7, ratioInString);
            sprintf(killsInString, "%d", server->player[player].kills);
            sprintf(deathsInString, "%d", server->player[player].deaths);
            strcpy(sendingMessage, server->player[player].name);
            strcat(sendingMessage, " has kill to death ratio of: ");
            strcat(sendingMessage, ratioInString);
            strcat(sendingMessage, " (Kills: ");
            strcat(sendingMessage, killsInString);
            strcat(sendingMessage, ", Deaths: ");
            strcat(sendingMessage, deathsInString);
            strcat(sendingMessage, ")");
            sendServerNotice(server, player, sendingMessage);
        }
    }
}