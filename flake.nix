{
  description = "Flake for PlatformIO development environment.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem(system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in 
        {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            avrdude
            platformio
          ];

          shellHook = ''
              export PS1="\[\033[1;34m\](nix-shell) \u@\h:\w\$\[\033[0m\] "
              '';
        };
      }
    );
}
