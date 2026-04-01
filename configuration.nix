# Edit this configuration file to define what should be installed on
# your system. Help is available in the configuration.nix(5) man page, on
# https://search.nixos.org/options and in the NixOS manual (`nixos-help`).

{ config, lib, pkgs, ... }:

{
  imports =
    [ # Include the results of the hardware scan.
      ./hardware-configuration.nix
    ];

  nix.settings.experimental-features = [
	"nix-command"
	"flakes"
  ];


  # Use the systemd-boot EFI boot loader.
  boot.loader.systemd-boot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;

  # networking.hostName = "nixos"; # Define your hostname.

  # Configure network connections interactively with nmcli or nmtui.
  networking.networkmanager.enable = true;

  # Set your time zone.
  time.timeZone = "Europe/Paris";

  # Configure network proxy if necessary
  # networking.proxy.default = "http://user:password@proxy:port/";
  # networking.proxy.noProxy = "127.0.0.1,localhost,internal.domain";

  # Select internationalisation properties.
  # i18n.defaultLocale = "en_US.UTF-8";
  # console = {
  #   font = "Lat2-Terminus16";
  #   keyMap = "us";
  #   useXkbConfig = true; # use xkb.options in tty.
  # };

  # Enable the X11 windowing system.
  services.xserver.enable = true;


  # Enable the GNOME Desktop Environment.
  services.displayManager.gdm.enable = true;
  services.desktopManager.gnome.enable = true;

  
services.udev.packages = [
    pkgs.platformio-core
    pkgs.openocd
];

  programs.dconf = {
  enable = true;
  profiles.user.databases = [
    {
    settings = {
        "org/gnome/settings-daemon/plugins/media-keys" = {
            custom-keybindings = [
                "/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/"
            ];  
        };
        "org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0" = {
            name = "Terminal";
            command = "kgx";
            binding = "<Ctrl><Alt>T";
        };

        "org/gnome/desktop/wm/keybindings" = {
            move-to-workspace-left=["<Shift><Alt>x"];
            move-to-workspace-right=["<Shift><Alt>c"];
            switch-to-workspace-right = [ "<Alt>c" ];
            switch-to-workspace-left  = [ "<Alt>x" ];
        };
        "org/gnome/desktop/peripherals/touchpad" = {
            click-method = "areas";
            two-finger-scrolling-enabled	= true;	
        };  
        "settings-daemon/plugins/color" = {
            night-light-enabled = true;
            night-light-schedule-automatic = false;
            night-light-schedule-from = 0.0;
            night-light-schedule-to = 0.0;
            night-light-temperature = lib.gvariant.mkUint32 3526;
        };
        "org/gnome/shell/app-switcher" = {
            current-workspace-only=true;

        };

        "org.gnome.desktop.wm.preferences" = {
            num-workspaces = lib.gvariant.mkUint32 2;
            
        };
        "org.gnome.mutter" = { 
            dynamic-workspaces = false;
        };


    };
      lockAll = true; # false si tu veux pouvoir modifier depuis GNOME
    }
  ];
};

  programs.nix-ld.enable = true;
  programs.nix-ld.libraries = with pkgs; [
    # Add any missing dynamic libraries for unpackaged programs
    # here, NOT in environment.systemPackages
 	avrdude
  ];

  # Configure keymap in X11
   services.xserver.xkb.layout = "fr";
  # services.xserver.xkb.options = "eurosign:e,caps:escape";

  # Enable CUPS to print documents.
  services.printing.enable = true;

  # Enable sound.
  #  services.pulseaudio.enable = true;
  # OR
  # services.pipewire = {
  #   enable = true;
  #   pulse.enable = true;
  # };

  # Enable touchpad support (enabled default in most desktopManager).
  services.libinput.enable = true;
  

  # Define a user account. Don't forget to set a password with ‘passwd’.
  users.users.raph = {
    isNormalUser = true;
     extraGroups = [ "wheel" "dialout" ]; # Enable ‘sudo’ for the user.
     packages = with pkgs; [
       tree
     ];
   };

  programs.firefox.enable = true;
  programs.bash.enable = true;
  programs.direnv.enable = false;


environment.gnome.excludePackages = with pkgs; [
    # baobab      # disk usage analyzer
    cheese      # photo booth
    # eog         # image viewer
    epiphany    # web browser
    gedit       # text eddiitor
    simple-scan # document scanner
    # totem       # video player
    yelp        # help viewer
    # evince      # document viewer
    file-roller # archive manager
    geary       # email client
    seahorse    # password manager

    # these should be self explanatory
    # gnome-calculator 
    gnome-calendar 
    # gnome-characters
    gnome-clocks
    gnome-contacts
    # gnome-font-viewer
    #  gnome-logs
    # gnome-maps
    gnome-music
    gnome-photos
    #  gnome-screenshot
    # gnome-system-monitor 
    gnome-weather
    # gnome-disk-utility
    #  pkgs.gnome-connections
  ];


services.usbmuxd.enable = true;

  nixpkgs.config.allowUnfreePredicate = pkg: builtins.elem (lib.getName pkg) [
             "vscode"
           ];



  # List packages installed in system profile.
  # You can use https://search.nixos.org/ to find more packages (and options).
  environment.systemPackages = with pkgs; [
 	vscode
	git
	dconf2nix
    direnv # pour les environnements de travail
	(python3.withPackages (python-pkgs: with python-pkgs; [
      	platformio
	numpy
	matplotlib
      	]))
	 gcc
	 gsl
	 gnumake
	 graphviz
    libimobiledevice

	nodejs
	nodePackages.pnpm

	libclang
 ];

  # Some programs need SUID wrappers, can be configured further or are
  # started in user sessions.
  # programs.mtr.enable = true;
  # programs.gnupg.agent = {
  #   enable = true;
  #   enableSSHSupport = true;
  # };

  # List services that you want to enable:

  # Enable the OpenSSH daemon.
  # services.openssh.enable = true;

  # Open ports in the firewall.
  # networking.firewall.allowedTCPPorts = [ ... ];
  # networking.firewall.allowedUDPPorts = [ ... ];
  # Or disable the firewall altogether.
  # networking.firewall.enable = false;

  # Copy the NixOS configuration file and link it from the resulting system
  # (/run/current-system/configuration.nix). This is useful in case you
  # accidentally delete configuration.nix.
system.copySystemConfiguration = true;

  # This option defines the first version of NixOS you have installed on this particular machine,
  # and is used to maintain compatibility with application data (e.g. databases) created on older NixOS versions.
  #
  # Most users should NEVER change this value after the initial install, for any reason,
  # even if you've upgraded your system to a new NixOS release.
  #
  # This value does NOT affect the Nixpkgs version your packages and OS are pulled from,
  # so changing it will NOT upgrade your system - see https://nixos.org/manual/nixos/stable/#sec-upgrading for how
  # to actually do that.
  #
  # This value being lower than the current NixOS release does NOT mean your system is
  # out of date, out of support, or vulnerable.
  #
  # Do NOT change this value unless you have manually inspected all the changes it would make to your configuration,
  # and migrated your data accordingly.
  #
  # For more information, see `man configuration.nix` or https://nixos.org/manual/nixos/stable/options#opt-system.stateVersion .
  system.stateVersion = "25.11"; # Did you read the comment?

}

