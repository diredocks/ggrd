{
  description = "ggrd, real-time face recognizer and streamer";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux"];
    forEachSystem = nixpkgs.lib.genAttrs supportedSystems;

    pkgs = forEachSystem (system:
      import nixpkgs {
        inherit system;
        overlays = [self.overlays.default];
      });
  in {
    formatter = forEachSystem (system: pkgs.${system}.alejandra);

    overlays.default = final: prev: {
      go = prev.go_1_24;
    };

    devShells = forEachSystem (system: {
      default = pkgs.${system}.mkShell {
        packages = with pkgs.${system}; [
          # build and dev tools of streamer
          cmake
          gcc
          clang-tools
          # dependency of building streamer
          opencv
          dlib
          zlib
          spdlog
          # http server
          go
          gotools
          golangci-lint
          gopls
        ];
      };
    });
  };
}
