{
  description = "PlatformIO + C dev";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs";

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = [
        pkgs.platformio
        pkgs.openocd

        # outils C
        pkgs.gcc
        pkgs.clang
        pkgs.clang-tools
        pkgs.gdb
        pkgs.cmake
      ];
    };
  };
}