{
  description = "C++ dev shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = with pkgs; [
        gcc
        clang-tools
        cmake
        gnumake

        sfml
        asio
      ];

      shellHook = ''
        export ASIO_INCLUDE_DIR=${pkgs.asio}/include
      '';
    };
  };
}
