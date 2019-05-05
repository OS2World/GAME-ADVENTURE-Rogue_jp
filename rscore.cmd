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
 * Basic Environments
 */

home = VALUE('HOME', saved,  'OS2ENVIRONMENT')
user = VALUE('USER', player, 'OS2ENVIRONMENT')

'@'rogue' -s'

PULL ack

junk = VALUE('HOME', home,  'OS2ENVIRONMENT')
junk = VALUE('USER', user, 'OS2ENVIRONMENT')
CALL DIRECTORY curdir

EXIT
