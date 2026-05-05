{
  description = "Pooooooooool flake";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";

  outputs = { self, nixpkgs }: let
    systems = [ "x86_64-linux" "aarch64-darwin" ];
    forAllSystems = nixpkgs.lib.genAttrs systems;
    pkgsFor = system: nixpkgs.legacyPackages.${system};
  in {
    packages = forAllSystems(system: {
      default = (pkgsFor system).callPackage ./package.nix { };
    });

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
