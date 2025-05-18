{
  description = "Dev environment for face recognition WebSocket project";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: {
    devShell.x86_64-linux = let
      pkgs = import nixpkgs {
        system = "x86_64-linux";
      };
    in pkgs.mkShell {
      packages = with pkgs; [
        cmake
        gcc
        clang-tools

        opencv
        dlib
        zlib
        spdlog
      ];
    };
  };
}
