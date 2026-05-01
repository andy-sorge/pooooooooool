{
  description = "C++ dev shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: let
    systems = [ "x86_64-linux" "aarch64-darwin" ];
    forAllSystems = nixpkgs.lib.genAttrs systems;
    pkgsFor = system: nixpkgs.legacyPackages.${system};
  in {
    devShells = forAllSystems(system:
      let pkgs = pkgsFor system; in {
        default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            clang-tools
            cmake
            gnumake
            sfml
          ];
        };
    });

  };
}
