/*
 * prepare rogueopts and invoke rogue
 */

CALL RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
CALL SysLoadFuncs

/*
 * configure - where is executable, save file to be placed
 */

saved = 'E:\games\games\jrogue'     /* PATH to save files   */
gamed = 'D:\akira\games\jrogue'     /* PATH to executable   */
rogue = gamed'\rogue'

/*
 * Rogue Options
 */

jump      = 'jump'
passgo    = 'passgo'
tombstone = ''
color     = ''
map       = 'map:cbMYB'

/*
 * Basic Environments
 */

PARSE ARG player fruit remain

IF player = '' THEN
    player = 'êÌém'
IF fruit = '' THEN
    fruit = 'Ç±ÇØÇ‡Ç‡'

home = VALUE('HOME', saved,  'OS2ENVIRONMENT')
user = VALUE('USER', player, 'OS2ENVIRONMENT')
sfile = player'.sav'

/*
 * Rogue Options
 */

opts = 'file:'sfile',fruit:'fruit

IF jump <> '' THEN
    opts = opts','jump
IF passgo <> '' THEN
    opts = opts','passgo
IF tombstone <> '' THEN
    opts = opts','tombstone
IF color <> '' THEN
    opts = opts','color
IF map <> '' THEN
    opts = opts','map

junk = VALUE('ROGUEOPTS', opts,  'OS2ENVIRONMENT')

/*
 * check if save file exists
 */

curdir = DIRECTORY()
CALL DIRECTORY saved
CALL SysFileTree sfile, 'found', 'FO'

/*
 * Start new game or restore game
 */

IF found.0 = 0 THEN
    '@'rogue
ELSE
    '@'rogue sfile

/*
 * Restore Environments
 */

junk = VALUE('HOME', home,  'OS2ENVIRONMENT')
junk = VALUE('USER', user, 'OS2ENVIRONMENT')
CALL DIRECTORY curdir

EXIT
