
[Setup]
AppName=TradeViwer
AppVersion=0.1
AppVerName=TradeViwer 0.1
AppPublisher=MySERVER project
AppPublisherURL=http://www.it-hobby.km.ua
AppSupportURL=http://www.it-hobby.km.ua
AppUpdatesURL=http://www.it-hobby.km.ua
DefaultDirName={pf}\TradeViwer
;SetupIconFile=TradeViwer.ico
DefaultGroupName=TradeViwer
DisableProgramGroupPage=yes
Compression=zip
SolidCompression=yes
UninstallDisplayIcon={app}\TradeViwer.exe
OutputDir=.
OutputBaseFilename=TradeViwer-setup-0.1

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "TradeViwer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libcurl.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libssl32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "jansson.dll"; DestDir: "{app}"; Flags: ignoreversion

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "LICENSE.txt"

[Icons]
Name: "{commonprograms}\TradeViwer"; Filename: "{app}\TradeViwer.exe"
Name: "{userdesktop}\TradeViwer"; Filename: "{app}\TradeViwer.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\TradeViwer"; Filename: "{app}\TradeViwer.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\TradeViwer.exe"; Description: "{cm:LaunchProgram,TradeViwer}"; Flags: nowait postinstall skipifsilent
