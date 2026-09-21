let
  pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-26.05") {};
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    platformio
    libc
  ];
}
