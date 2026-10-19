let
  pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-26.05") { };
  fontsConf = pkgs.makeFontsConf {
    fontDirectories = [
      pkgs.profont
    ];
  };
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    platformio
    libc
  ];
  shellHook = ''
    export FONTCONFIG_FILE="${fontsConf}"
  '';
}
