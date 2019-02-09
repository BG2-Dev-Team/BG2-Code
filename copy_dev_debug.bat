@echo off
echo Copying to game directory
copy "%CD%\game\client\Debug_mod_hl2mp\client.dll" "C:\Program Files (x86)\Steam\SteamApps\sourcemods\bg-2013\bin"
copy "%CD%\game\server\Debug_mod_hl2mp\server.dll" "C:\Program Files (x86)\Steam\SteamApps\sourcemods\bg-2013\bin"

@echo Copying debugging symbols
copy "%CD%\game\client\Debug_mod_hl2mp\client.dll" "%CD%\dumps"
copy "%CD%\game\server\Debug_mod_hl2mp\server.dll" "%CD%\dumps"
copy "%CD%\game\client\Debug_mod_hl2mp\client.pdb" "%CD%\dumps"
copy "%CD%\game\server\Debug_mod_hl2mp\server.pdb" "%CD%\dumps"

pause