README for Lightweight2:

--- About:
    Lighweight2 is a very small Browser. It is just for surfing the web.
    It supports no extensions. It has almost no config options.
    The only confirable things are:
    
      - Default searchengine
      - Costum searchengines
      - Bookmarks
      - Allowed hosts to save cookies
      - Allowed hosts for plugins

    As all modern Browsers, Lightweight2 also tries to use much space as
    possible for the webcontent.

--- Licence
    GPLv3
    See file copying.txt

--- Version
    1.9

--- Features
    - Very compact and easy to use inferface
    - Almost no text
    - No unnessesary messageboxes. Just surf in the web
    - On-the-fly hashcheck of downloads
    - Intelligent "New Tabs": It does not open tabs for urls that are already open

--- How to compile:
    You need Qt 4.6+ and its developer packages to be compiled.
    You also need libQtWebkit (or qt4-webkit) and its developer package.

    If you are using Pandora:
    - In src/lightweight.cpp:
      - Uncomment line 30 to enable alternate settings saving. QSettings does not work on this OS

    qmake
    make 

--- Run
    ./Lightweight2

--- Any Ideas or problems?
   - Mail me: anubis1@linux-ecke.de
   - Leave a comment on http://qt-apps.org/content/show.php/Lightweight2?content=139266
   - Write me a message: http://qt-apps.org/messages/?action=newmessage&username=cpttom

--- Files from others:
    - icons/search/google.png
      - From:     Joakim Jardenberg
      - Found in: http://www.flickr.com/photos/jocke66/4355060241/

--- Work by others:
    - Salvador Parra Camacho:
      - Added icons/icon_os2.ico
      - Added icons/icon_win.ico (Replaced by a new 24Bit RGB + 8Bit-Alpha icon)
      - Added Lightweight2_os2.rc
      - Added Lightweight2_win.rc
      - Added readme.os2
      - Included both *.rc files to Lightweight.pro
      - Spanish translation

    - "tetris4"
      - Greek translation

    - Jens Bache-Wiig:
      - src/os/qtwin.h
      - src/os/qtwin.cpp
      - Both found on http://labs.qt.nokia.com/2009/09/15/using-blur-behind-on-windows

--- Changelog:
    - 1.9
      - Fixed bug that allowed having two bookmarkfolders with the same name.
      - Authentication dialog: Changed Radio-button to Check-box
      - New content notification on non active tabs
      - Show SSL status on non active tabs
      - Added Tab preview in tooltip on tabbutton
      - Reduced chache to 128MiB to reduce first page-loading time
      - Tabmenu: rearrangend entrys to group the actions
      - Added ability to print pages
      - Added ability to use QWebInspector to view page-sources and develop with it
      - Added ability to use QtWebKits private browsing mode
      - Fixed bug downloading file into Downlaods dir, if filename is given in seleceted dir
      - Fixed bug not verfifying downloads wich prevented overriding

    - 1.8
      - Do not ask for downloads with invalid content-header
      - Fixed seeing progressbar multiple time
      - Added ability to rename bookmarks and bookmarkfolders
      - Refactor: Moved common parts of Bookmark and BookmarkFolder into BookmarkItem
      - Refactor: Above with BookmarkButton and BookmarkFolderButton into BookmarkItembutton
      - Added ability to change URL of a bookmark
      - Added ability to move bookmarks and folders

    - 1.7
      - Made topbar smaller to 24px
      - Moved new tab button right of the tab-buttons
      - Changed visibility of URL-bar on opened new tabs
      - Changed "Save to..." icons in AskDownload dialog to the green version
      - Added ability to save all cookies (Enabled by default)
      - Added ability to allow plugins for all domains (Enabled by default)
      - Fixed problem on SSlErrors that makes all tabs red. Now scan the whole tab for the correct reference
      - Fixed potential crash when opening Bookmarkmanage when viewing downloads
      - Added tooltips to the delete buttons in the bookmark-manager
      - Added tooltip for stop/remove download button
      - Greek translation

    - 1.6
      - Use Aero glass on Vista/7 if aviable
      - New buttons for deleting bookmarks/folders
      - Added page-serach entry into the tabmenu
      - New close icon
      - Improved handling of page-search-bar:
        * Pressing enter in text-field now searches the first/next matching
        * Shortcuts for the searchbuttons: F3 next matching; Shift+F3 previous matching
      - Added ability to hide/show the Lightweight2 button
      - Tab-button.menu: Removed checkbox
      - Prevent of closing last tab. Going to about:blank instead
      - Ability to open bookmarks in in an open tab
      - Hide "Create bookmark" on empty page
      - GUI-prevention of creating empty page bookmarks
      - Save visibility of LW2 button
      - Aks for download-target for downloads
      - Go to last showed tab, when removing last download from list
      - Don't show empty folders in new-tab menu and goto-menu
      - Don't show open all button, if bookmark-folder has only one bookmark

    - 1.5
      - Support for webplugins (Enable allowed plugin-hosts via button)
      - Added dialog to manage allowed hosts for plugins
      - Set focus on WebView on Tab-Change
      - Added Lightweight2-menu button
      - Removed format on load-progressbar
      - Fixed crashed caused by deleting sumDl in its event-loop (in download.cpp)
      - Added dialog to set what domains to save cookies for

    - 1.4
      - Changed all flat PushButtons to ToolButtons with AutoRaise:
        HoverFeedback seems only be supported by Oxygen
      - Moved Hover url into WebView
      - Added ability to open file after downloading it
      - Fixed some optical issue with the display of teh remaining datasize
      - Open "mailto:" Urls with desktop-email-client
      - Indicate no active tab, when opening downloads-view
      - Added QSettings workaround for Pandora
        * To use it uncomment line 30 from src/lgihtweight.cpp 
      - Bookmarkmanager: Fixed bug that caused displaying wrong buttons

    - 1.3
      - Added more shortcuts:
        * Ctrl+Left / Ctrl+Right : Go to tab left / right of current tab
        * Ctrl+E : Show/Hide urlBar of current tab
        * Ctrl+W : Close current tab
      - Indicate the current acive tab
      - Moved pageloading progressbar to the buttom
      - Added ability to save cookies on disk (User must give the allowed domains)
      - Fixed bug: Always show a download in progrss after stopping a download
      - Open bookmark: Show a tab with the bookmark-url, if there is one. Do not create a new tab in that case
      - Add ability to create a new bookmark in bookmarkmanager. (Creates a bookmark from the current viewed page)
      - Allow going to "about:blank"
      - Added loading of translations from an Unix-instllation
      - Added support for disk-cache up to 1GiB

    - 1.2
      - Show loading progress
      - On-the-fly SHA1 check of downloads (If SHA1 sum is aviable)
      - Searchbar
      - Complete new bookmark system with folders
      - Ability to open urls given as arguments

    - 1.1
      - Download support
      - On opening bookmark: Use the bookmark icon as icon
      - Fixed compile problem with Qt 4.6 (Qt 4.6 does not know Qt::Middelbutton
      - Added icons for web-contextmenu
      - Translate web-contextmenu
      - Fixed possible crash when using control-buttons with no tab in Window
      - New README layout
      - Make use of Qt given translations
      - Translation: Spanish
      - Disable useless control-buttons
      - Show only reload or stop button
      - Save and restore last Window-position and size
      - Set Focus on Urlbar on new empty tab

    - 1.0
      - Translation support: German
      - Authentication support
      - Support for JS popups
      - Middleclik opens link in new tab
      - Changed programicon for Windows

    - 0.5
      - Bookmark support
      - Costum searchengines
      - Show URLs of hoverd links
      - Shortcuts
      - "Open link in new tab" support
      - Added Programicons for OS/2 and Windows

    - 0.1
      First release

--- How to
    - add a bookmark:
      - Click and hold the tabbutton.
      - A contextmenu appears
      - Click on "Bookmark"
      - Navigate to the place where you want to have the bookmark
      - In the wished folder click on "Add here"

      or:

      - Press Ctrl+B
      - Navigate to the place where you want to have the bookmark
      - Click on "Create bookmark from current page"

    - open a bookmark:
      - Click and hold the "New tab" Button
      - A contextmenu appears
      - Click on the wished bookmark

    - delete a bookmark:
      - Press Ctrl+B
      - Navigate through the folders and click on the red x to delete a bookmark or folder
        Warning: Deleting a folder will cause deleting all bookmarks and subfolders in it!

    - create a costum searchengine:
      - Go to Home/.lightweight2/searchengines
      - Create a new directory (choose any name)
      - Go into that directory
      - Create a new file called 'info' (No extension!!)
      - Write in one line the URL to be used for searching. Use %{query} as placeholder
          for the actual search
      - Create an icon called "icon.png" for it.

    - use a different searcheinge:
      - Click and hold the small box left of the searchbox
      - Choose the icon for the wished searchengine

    - allow a domain to save cookies
      - Go to Home/.lightweight2
      - Create a file named "whitelistCookies"
      - For each domain you want to save cookies:
        * Write in an own line the domain you want to allow
          For example: For "http://www.website.com/deeper/path" write ".website.com"
          or "www.website.com" (If you only want cookies from the www subdomain)
        * When closing Lightweight2 the cookies are saved to disk. Only those cookies are saved
          wich domains ends with one of the given allowed domains.
          WARNING: If you forget the . or the subdomain, Lightweight2 will also
          save all cookies from http://www.evilwebsite.com

     - disallow a domain to save cookies
      - Go to Home/.lightweight2
      - Open "whitelistCookies"
      - Remove the line with the domain to disallow
        WARNING: When you disallow a domain to save cookies its cookies will be deleted the
                 Next time you close Lightweight2

    - allow plugins for a host
      - Go to Home/.lightweight2
      - Create a file named "whitelistPluginHosts"
      - For each host you want to allow cookies:
        * Write in an own line the host you want to allow
          For example: For "http://www.website.com/deeper/path" write "www.website.com"
 
     - disallow plugins for a host
      - Go to Home/.lightweight2
      - Open "whitelistPluginHosts"
      - Remove the line with the host to disallow

--- How
    - does it check Downloads on-the-fly?
      If you start downloading for example http://www.website.com/files/bigfile.dat
      Lightweight2 will take two attepts to get the SHA1 sum:
      1: Try to get http://www.website.com/files/bigfile.dat.sha1
         It will take the first 40Bytes of bigfile.dat.sha1 as the checksum
      2: Try to get http://www.website.com/files/SHA1SUMS
         It will serach the line that gives the wished checksum 

--- TODO
    - ?? Massive tabbing
           "Scrolling tabs" (Don't use QScrollarear. It wastes space)
       or: Move some tabs into an context menu
       Only needed if too many tabs are open
    - ?? Remove default Serachengines