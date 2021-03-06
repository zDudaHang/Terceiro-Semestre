#!/bin/bash
# Usage: grade dir_or_archive [output]

# Ensure realpath 
realpath . &>/dev/null
HAD_REALPATH=$(test "$?" -eq 127 && echo no || echo yes)
if [ "$HAD_REALPATH" = "no" ]; then
  cat > /tmp/realpath-grade.c <<EOF
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
  char* path = argv[1];
  char result[8192];
  memset(result, 0, 8192);

  if (argc == 1) {
      printf("Usage: %s path\n", argv[0]);
      return 2;
  }
  
  if (realpath(path, result)) {
    printf("%s\n", result);
    return 0;
  } else {
    printf("%s\n", argv[1]);
    return 1;
  }
}
EOF
  cc -o /tmp/realpath-grade /tmp/realpath-grade.c
  function realpath () {
    /tmp/realpath-grade $@
  }
fi

INFILE=$1
if [ -z "$INFILE" ]; then
  CWD_KBS=$(du -d 0 . | cut -f 1)
  if [ -n "$CWD_KBS" -a "$CWD_KBS" -gt 20000 ]; then
    echo "Chamado sem argumentos."\
         "Supus que \".\" deve ser avaliado, mas esse diretório é muito grande!"\
         "Se realmente deseja avaliar \".\", execute $0 ."
    exit 1
  fi
fi
test -z "$INFILE" && INFILE="."
INFILE=$(realpath "$INFILE")
# grades.csv is optional
OUTPUT=""
test -z "$2" || OUTPUT=$(realpath "$2")
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
# Absolute path to this script
THEPACK="${DIR}/$(basename "${BASH_SOURCE[0]}")"
STARTDIR=$(pwd)

# Split basename and extension
BASE=$(basename "$INFILE")
EXT=""
if [ ! -d "$INFILE" ]; then
  BASE=$(echo $(basename "$INFILE") | sed -E 's/^(.*)(\.(c|zip|(tar\.)?(gz|bz2|xz)))$/\1/g')
  EXT=$(echo  $(basename "$INFILE") | sed -E 's/^(.*)(\.(c|zip|(tar\.)?(gz|bz2|xz)))$/\2/g')
fi

# Setup working dir
rm -fr "/tmp/$BASE-test" || true
mkdir "/tmp/$BASE-test" || ( echo "Could not mkdir /tmp/$BASE-test"; exit 1 )
UNPACK_ROOT="/tmp/$BASE-test"
cd "$UNPACK_ROOT"

function cleanup () {
  test -n "$1" && echo "$1"
  cd "$STARTDIR"
  rm -fr "/tmp/$BASE-test"
  test "$HAD_REALPATH" = "yes" || rm /tmp/realpath-grade* &>/dev/null
  return 1 # helps with precedence
}

# Avoid messing up with the running user's home directory
# Not entirely safe, running as another user is recommended
export HOME=.

# Check if file is a tar archive
ISTAR=no
if [ ! -d "$INFILE" ]; then
  ISTAR=$( (tar tf "$INFILE" &> /dev/null && echo yes) || echo no )
fi

# Unpack the submission (or copy the dir)
if [ -d "$INFILE" ]; then
  cp -r "$INFILE" . || cleanup || exit 1 
elif [ "$EXT" = ".c" ]; then
  echo "Corrigindo um único arquivo .c. O recomendado é corrigir uma pasta ou  arquivo .tar.{gz,bz2,xz}, zip, como enviado ao moodle"
  mkdir c-files || cleanup || exit 1
  cp "$INFILE" c-files/ ||  cleanup || exit 1
elif [ "$EXT" = ".zip" ]; then
  unzip "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".tar.gz" ]; then
  tar zxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".tar.bz2" ]; then
  tar jxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".tar.xz" ]; then
  tar Jxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".gz" -a "$ISTAR" = "yes" ]; then
  tar zxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".gz" -a "$ISTAR" = "no" ]; then
  gzip -cdk "$INFILE" > "$BASE" || cleanup || exit 1
elif [ "$EXT" = ".bz2" -a "$ISTAR" = "yes"  ]; then
  tar jxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".bz2" -a "$ISTAR" = "no" ]; then
  bzip2 -cdk "$INFILE" > "$BASE" || cleanup || exit 1
elif [ "$EXT" = ".xz" -a "$ISTAR" = "yes"  ]; then
  tar Jxf "$INFILE" || cleanup || exit 1
elif [ "$EXT" = ".xz" -a "$ISTAR" = "no" ]; then
  xz -cdk "$INFILE" > "$BASE" || cleanup || exit 1
else
  echo "Unknown extension $EXT"; cleanup; exit 1
fi

# There must be exactly one top-level dir inside the submission
# As a fallback, if there is no directory, will work directly on 
# tmp/$BASE-test, but in this case there must be files! 
function get-legit-dirs  {
  find . -mindepth 1 -maxdepth 1 -type d | grep -vE '^\./__MACOS' | grep -vE '^\./\.'
}
NDIRS=$(get-legit-dirs | wc -l)
test "$NDIRS" -lt 2 || \
  cleanup "Malformed archive! Expected exactly one directory, found $NDIRS" || exit 1
test  "$NDIRS" -eq  1 -o  "$(find . -mindepth 1 -maxdepth 1 -type f | wc -l)" -gt 0  || \
  cleanup "Empty archive!" || exit 1
if [ "$NDIRS" -eq 1 ]; then #only cd if there is a dir
  cd "$(get-legit-dirs)"
fi

# Unpack the testbench
tail -n +$(($(grep -ahn  '^__TESTBENCH_MARKER__' "$THEPACK" | cut -f1 -d:) +1)) "$THEPACK" | tar zx
cd testbench || cleanup || exit 1

# Deploy additional binaries so that validate.sh can use them
test "$HAD_REALPATH" = "yes" || cp /tmp/realpath-grade "tools/realpath"
export PATH="$PATH:$(realpath "tools")"

# Run validate
(./validate.sh 2>&1 | tee validate.log) || cleanup || exit 1

# Write output file
if [ -n "$OUTPUT" ]; then
  #write grade
  echo "@@@###grade:" > result
  cat grade >> result || cleanup || exit 1
  #write feedback, falling back to validate.log
  echo "@@@###feedback:" >> result
  (test -f feedback && cat feedback >> result) || \
    (test -f validate.log && cat validate.log >> result) || \
    cleanup "No feedback file!" || exit 1
  #Copy result to output
  test ! -d "$OUTPUT" || cleanup "$OUTPUT is a directory!" || exit 1
  rm -f "$OUTPUT"
  cp result "$OUTPUT"
fi

echo -e "Grade for $BASE$EXT: $(cat grade)"

cleanup || true

exit 0

__TESTBENCH_MARKER__
� `�[ �]S�H2���	2X��GHBL `�dI��ݪ���%���H@v��l���=�����u�H�$K�d���[wQ��t�����H0�2�{V���Aaue���ՕZ�ý�����ǵ�"���V����_'�B�>�=�a�6/ƻm��������
>��V�f��M ��}ߴ�~a��כ��_]����J�~j_O�b�������Sۭ���L9��n���vs���(-*[�GoۍҒ�q����U����4��/7��o�V2�]PK�S��`�R��y�G�y>�`�P�8�J�gy,O�cC(���v�A�[��G���̏��L�{� "����Ȉu�<P<�o�}��� ��o����`�]�0�re���V��� w<�-��Z��1�ͤ��G)���٣u�\����Ӛl�g(�AM��?��qJ}��>󿆍�������������	$�_yu���<�?jS��L�/*�q�@�p��������9��>;��a��9L]C�K�H�/f�c�:5��qJH�{wa~��>{�����#��D��g<t����7����\��]��{���1r7�O�nW��Y�w=7`n Z~C���W�oO�%��cћglԍ�����T\�"�|��{P�z��g̴(����cz#n"��,}Wx|����"0m�J��.���x�I���������;@���zF�n�4�DÂ\���ě9���'(U����P~�n�ݨ+͟�q2RKj��rJ�\pr1�xQ���Z��ZJ
�B^J!���(�;���!i&�{Ԓ���=9<�
� X_�X�PL,Fz�y@"��%TL�4���#��e2�њW�/#Ъ��^l�6�m7�����X��3]�e�j���ˠ6�ĥ�[�,<�ztr���c]=^��0ɡ�EW�@�ő�ބ9^=6j�j�?WVa=(������q�o��i�N��s��AI���rA��/Sm����.�����!�̆X�a�w3��m���5��Jp��}�D�3f��<�����*��V�4���Z�C�ځ1��]b鍄n`:��.��t������/Q�������R��a`;Ʀ�l�M�<�H�}�HȘ;l����ⲋ���h<���ўq�{]ƹg�����$�s�Q=������*�J�X'F�19/+���AJqIo%@�x4�Dk���O�@��sClLA�g�q66{��$��gc�f���]�X�g�w��y�eCQ�'�Ʌ��⡂�.\����=f��z&&��v,ZÚ����@s� �k�:,*�Ұ��|߶��HRn4�\j9������,C���Z����z]���D|?����,��:�:RO���!��#Mw].P��-3��9�ђť��5��! �柞<{!�0�p��0�xCp��V
u�Y̭bL��[��Z����RJ�~��uº�e��M q����΃��K�Bf2��?Va��~��h�7-��bYqB2�7^�c�@�W ���5e�B�o��7�0��V��7�[�ۻ�婺�!���l�{�����ݷn�#u�E���q�A*���a:���7�����ƛ͟N~��;j�M%<�۟M�*�˭Q`Z}JByDa�l^F�:C��	w͠S�yѬG�J��(k=_'�M�K�ɀm�:�v���Yp4LbrT��+��*�T*2�f�M�@1[��&�`<����y�t���@�ֿ,F)Pǹ=�� �Kҏ���b���j�<^\���v�a�C6)UlQdIU��f�lbOwB<���ޯCO<(��<6����}0�u���gF�XkW��Y�1W#˷�������/E�p�YAf!��Z�`ϯ�3�/h�����R����\�aZ��.sm%�KaR���t����;�%M�L0=�'!z�25j�Vv��������<�������O�(�.Z[>�؈�����I���q�|��S��|%� �0�,��,2B�p�Lp��3�r�j�ʇ�y���!�T����&��0��6$C�R���l���<6��2��T��V�BO�)���@eV>��ݽ��Vsk��v����[��a-m�w(��Nܱ�"��ڊ����*�F�E+��)�|a������9�-oB9'��1� ��0��xCo�E,�<: B;�`h�&Va>��05��".Y�������a�E�.����m&i�UP�v���S�/��s�7
h#�簺R�m��U�top�s�Ӵa	�Ӊj�vb�Iu��9
,�9���D	��(q������@��q�W�#[�y6�;^�nh���Z��U
�I%��F\ĉ�X�MH�ģ���˅���˩�r��C+e����͗���(�].~M�5A;���_����V�s9��h��e��@x�xF�Ӵ�g�|��G��	�%����r:?���б{6��)bK}��tM�sw�*�M�z�F#.����Q�P��#|��*��S��k��w��i��e�_���<L�ޞ�
q2J�jI��g�c��~YS�DZ��:�e�"օ�RB%�Ȭ��p��GTE�ݙ�I��e��?'L?�%�F�S1Rt?�
m'���)�RY�rtc�.��sӵ^�b,%ln�z�"cd_��4{n:!C��	k�����	D�ƤC���3IwqF��59�z�"/�P�N_�hi�ek���\P�,�&+�/7�����w�v�>���J��6���:�����
��'`?��ߘn��Y^}�������:���- ��ӵ�0���F1/���UzX���_��]��׮j�݅>��*��/����1��爅��s7n���H�h�~�6ۍ�%�PKx?�e ��t��P���6��Q#�ȿ�/��<�-o�n�0E��+�=ـP��1^�hܐ����}T�A��ؕ�cΞn\L��19LY�Il/�1Bx��k���}��b���5Oz�h�$L,���IUH,:��>��B<3��d���h-0��m�5c�+5��I$�3��Q�J��j�+��yx(�l��5���~�࠹-~�Oo����<�Y����3��ʫ4��|�M�8\��x�S��P�7�ɔي�T�v���x�2VJ�}<Q9�^}���L��E�ËT�+�5w�f�3�v(ѵ�1OJ�U{Wӟ�/��|�z\���B�oVBSYr����F/�e@�@/���"ޙ,���u��2z.��@��%!��Qh�P�f�hҪ�rZ\x2�i�K\tI��4��U��3-�S�L�h����9=M���T�SN<�v�h^mԂ7�ީ�'>5>5@�גM������\��,0tݐ�sǫ��X��C�BRAobTO��(W�9��趣��\�N� �!�zE�u���*�c$����m(��c�J��ŏ?���EP���
���C����E��8�L�m
���R��{�6�c���RH�#�Ri� uWg�=#x'ګN�"�t�4��jz�"��1߫���1����:'�'P*���d���(m�a�8��u: uI��\4�AG����5�J$$���#���v�#���#56��cgn��f%�}Nʾh��o�EmCs�fp�9�|Ӷ=�_)��x�9�s?0�+�qf��e�v-YP�~��e�\��<��ܴ1�`E t��ϥ=���������PEe ����q3��f0��`3��f0��`3��f0��o�/���� P  