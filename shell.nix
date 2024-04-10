{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  name = "group3";
  buildInputs = with pkgs; [
    cmake
    gnumake
    gcc13

    doxygen
  ];
}
