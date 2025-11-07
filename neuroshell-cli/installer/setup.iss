; NeuroShell AI CLI Installer Script for Inno Setup
; Download Inno Setup: https://jrsoftware.org/isdl.php

#define MyAppName "NeuroShell AI CLI"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Sid0004"
#define MyAppURL "https://github.com/Sid0004/neuroshell-ai-cli"
#define MyAppExeName "neuroshell.exe"

[Setup]
AppId={{NEUROSHELL-AI-CLI-UNIQUE-ID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\NeuroShell
DefaultGroupName={#MyAppName}
OutputDir=installer
OutputBaseFilename=NeuroShell-Setup-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
SetupIconFile=assets\icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "addtopath"; Description: "Add to PATH (allows running 'neuroshell' from anywhere)"; GroupDescription: "System Integration:"
Name: "addcontextmenu"; Description: "Add 'Open NeuroShell here' to folder context menu"; GroupDescription: "System Integration:"

[Files]
Source: "build\bin\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "config\*"; DestDir: "{app}\config"; Flags: ignoreversion recursesubdirs
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Registry]
; Add to PATH
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Check: NeedsAddPath('{app}'); Tasks: addtopath

; Add context menu
Root: HKCR; Subkey: "Directory\Background\shell\NeuroShell"; ValueType: string; ValueName: ""; ValueData: "Open NeuroShell here"; Tasks: addcontextmenu
Root: HKCR; Subkey: "Directory\Background\shell\NeuroShell"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: addcontextmenu
Root: HKCR; Subkey: "Directory\Background\shell\NeuroShell\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"""; Tasks: addcontextmenu

[Code]
function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;
