# WARNING - Deprecated
This tool should not be used.  It will remain available and receive compatibility updates for current FFXI client as needed, but users who are not already invested in it should look into:
https://github.com/ThornyFFXI/LuAshitacast - Recommended tool for this.  If you don't have any existing files, start with this.
https://github.com/ThornyFFXI/LegacyAC - If you are coming from Ashita3, or have Ashitacast XML files you've been given, you almost certainly need LegacyAC, not this repo.

## A note to people who use this anyway
LuAshitacast is a much more efficient and clean tool for the same purpose.  XML is not well-suited to programming tasks, and while there are improvements that could be made such as preprocessing into jump tables, Ashitacast at present does far too many string comparisons.  This is unlikely to matter on a modern processor, but extremely complex files may end up wasting some cpu time.  The lack of access to looping iteration and other basic functions a real language would have also limits your ability to customize things to extremes.  I highly recommend that anyone who is just getting started(and even those with some existing knowledge) consider looking at LuAshitacast instead.

# Ashitacast
Ashitacast is a plugin designed to manage your gear swaps.  This is configured in an XML file, which should be placed in *ashita/config/ashitacast/* and named *CharacterName_JOB.xml*.  
Note that this file only outlines commands, and you should reference the additional material in *ashita/docs/ashitacast/* when creating your XML file(s).

### Commands
All commands can be prefixed with **/ashitacast** or **/ac**.<br>
Any parameter that includes a space must be enclosed in quotation marks(**"**) unless part of /ac eval.

**/ac import [required: filename]**<br>
Attempt to convert an Acv3 XML to Acv4.  Filename must be in the format Charname_JOB_v3.xml.  Converted XML will be created as Charname_JOB.xml.  For more details, see quickstart.html in the ashita/docs/ashitacast folder.

**/ac load [optional: filename]**<br>
Attempt to load an XML.  Ashitacast will first check for an absolute path, then check ashita/config/ashitacast.  If you do not specify filename, ashitacast will use your current CharacterName_JOB.xml.

**/ac reload**<br>
Reload the currently loaded XML from hard drive, to reflect any changes made.

**/ac unload**<br>
Unload the currently loaded XML, resulting in no XML loaded.

**/ac naked**<br>
Remove all equipment and disable all slots.

**/ac set [required: set name] [optional: duration]**<br>
Equip a set from your current XML manually.  It will be locked in for the specified duration, in milliseconds.  If duration is not specified, it will be locked in for 5000ms(5 seconds).

**/ac disable [optional: slot]**<br>
Temporarily disable a slot from being swapped by ashitacast.  If no slot is specified, all slots will be disabled.

**/ac enable [optional: slot]**<br>
Enable a slot that has been disabled so that it will be swapped again.  If no slot is specified, all slots will be enabled.

**/ac print [required: vars|uvars]**<br>
If vars is specified, print a list of all currently accessible integrated variables and their values.  If uvars is specified, print a list of all user-defined variables and their current values.

**/ac addset [required: set name]**<br>
Add your currently equipped armor to your XML as a set.  If a set with the listed name already exists, it will be replaced.  Your XML will be reloaded afterwards, so the set will take effect immediately.

**/ac newxml [optional: filename]**<br>
Create a new empty XML in ashita/config/ashitacast/.  If filename is not specified, ashitacast will use your current CharacterName_JOB.xml.  Will fail if the file already exists.

**/ac setvar [required: name] [required: value]**
Set a variable.  You may use math and reference other variables in value, by the same rules as advanced tags.
 
**/ac clearvar [required: name]**
Clear a variable.
 
**/ac clearvars**
Clear all variables.

**/ac debug [optional: on/off]**<br>
Set debug mode.  When debug mode is enabled, all swaps will be printed to the chat log.  If on or off is not specified, mode will toggle.

**/ac validate**<br>
Use the plugin Packer to confirm all equipment in your sets section is currently in your equip-friendly bags.  Note that Packer has not yet been released for Ashita4, so this command will not do anything yet.

**/ac gear**<br>
Use the plugin Packer to collect all equipment in your sets section according to your configured settings in Packer.  Note that Packer has not yet been released for Ashita4, so this command will not do anything yet.

**/ac help [optional: command]**<br>
Request a command's parameters from ingame.  This will match the text here.  If no command is specified, a list of commands will be printed.

**/ac eval [Content]**<br>
This tag is unique in that it does not use arguments, so you do not need to include any quotes when creating your content.  This will parse all variables in your content then send the resulting command through.<br>
For example, if you were on the character Thorny and typed out: */ac eval /p Hi, my name is $p_name!*<br>
Ashitacast would send the command: */p Hi, my name is Thorny!*
