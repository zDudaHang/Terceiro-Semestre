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
� �[ �9	X׺XWFq�ת�z�B����,Q�ME�!�@����E@�j�n�j�*�u�h�m��K��Ukqk���V����V�����L���>��{��|�d�����3f�1gR�F���]�p���,����n��d~~����dn�2�_�����Xj�,��4!�F�|-�t�g��z���7Ӵ��S��?��������K���W�H%�#���G�Y�hj���}~,<��/��g?i�� �$1:*"�/#"1I��|9?6.)����H����H�P("�Bã���v$6 ߶�CEE�"UQ�R�E�#�h�"��5�Y�� ��	�E���F���A!vo��"sm�m<��5�QJ�x�i�9�N.�Cf���-f�#e"*�I-�,zd�-JZ"��`�K^Y�m7c��L#Ĳ���b��z�p?�*2S���fs��S<�C��U΢�ך�ˤ%���/a�����b=eRS���cψ(�M�� ����/����H���Ӏ�˸g9�#.�����x(�I��jFkP#֏���b��Q� �?�蟎&��T��2��ar�FL��cRM"�/�e:�[	�&�����2�h#m2ki�Kq�&�J��h7��Ȳ��65e+i��2��@�
,L12*.=&�/P� ��H6��Ç{zz�e�<A�C^	̈���U��Hζ2!�hL�Vll�5Qf���F�".�%a&�:8�/�`ȄB��,�L
L�	|�0,%G=w� jÄ_���6(!�M���x%�Kq�a�c��� ��,#�G'�6#8Ϛ�Yq����°��,[NP��P~��;�9�y0+�;���G7ᢇ���<+��k{T`�BB���*�Ɔ3-��u��b�H�\zyq���d�	`�����*2ȳA�F��`�B���yV*O�XJ�<!�<�0M�|��"ƒ&�T�L#%�� � � ��n���)�8|���`�b �X��i���W*U{y(�)Qĳ3���X��ʯ��៟��<'������2y��?@�w��+.����K?*�)�6��&�F�K��� �00ņ�I��_hA;��Ј�Eh��ФH\�p����2y N6l�h�m؟c���y��xڱ�(I3�Ԭ7Jm�Y�%J4t�"~�5(uf(cV鴙M��n��L���ix:0�ҤV��RC�||�C.W���J�.��҂���6"e�l��]�Sz�2�e�!�JDX�LC��p�C�Y���?��J5@�e���	�����/����ߊԦ�#��$�Q���ϰ8�o9�V��,B����d�y��c���V }(qo��R�����}����ƴ+4��,=�xEō��Q���G��}'<9"=:,�Re��j�ġ�q�o��<n�j|R���0��Ε�p�'a�d(=V�Eu�f$�T����%Z�M���'I�!
6��K괰[����ʰ�H��g����f��h�\��K�s��E%&�(��&��L�}�)%����Jx.�D�P�6S�Vy�Ŧ`�o5B
!9�a$J&�hM�Iuw4LЩ5◳=���>9����M� )U��cH=n�
�B#���&�+ �yW��9�5�`�	�dh�EZ3k�+Fi��DR�b4>� �@�X�@"��F�%&�&$a��� Ĺ0Ѩ����I@�Ц1 6(������*���~�]y�u?��B���LH|��T�@Y4Ek,��ߔ*��eN��O
�|i��Dx@�d�����Y�8A5��(�6��Y��5�&�cC���co`��l�P�>�\��B���TP1 ;��Ma�������8l���x�uS8��+��Ц�Q���&��aKƳ-��sFcs��4!mx\�5(z���w��|��jc�E�0G��ӂoM��RA/G�1<?�֪��p�46��B&����A��f���s	Fi�M�3t	Vt�!�(�}�#T��,�w��}���RT>�Qd|�"X���P0����#3@��[֦���z6��r �:d�rLM'^v����P@oasȱ#��K�2��0*ic�z��O�E	ò������9a��(���X�l~N�LZ�DZ����VI�r&[��4�(%(�S/hWH�Jn�_OBN���Ը$)TOAL����OV4�4WD�HO�*�=��!�ۇq)
's��VX)�p�\n��Qr���L=���)�Y�)��?�+�[��*,�a��ޘ���.P;��jl�R����8{�n*�N]I�<�
��� ���,D�S��<,����S�c��C���!�@��Ē��r<��DI��)�%�$eR��T�!��
�$�XG�/&89h���Y�ͳH��E�p�7�!�Cε�͸�.�T`p.�"�	�����hJ�G�m,�y�r>Z�Qj�Y�Ė�,Ȧ���z�SF�x2�M�������瞹��LJ�H��cC�����H�xC=��ڎ��&�PL6:!a-�,�X��n�mF�Œ�,ڤ���Z��!E��A���,����6��%�@�i,���KYv޹A�5�W�7��:����u��)� 4E��@�ݟ�Wj�Dh��Ug���t l=.'5�IOOR$&�)��#�x	ъ��to�o��ϯ��k!��;���)n�]�pAu4�C��r��H��&��
Ҍ�"͔�р�p�����j����jy�79�*��^��)<>�:NM,8S�����!�Hy��xɬ��:Z-|���&���k�8sӛc��c�Y칉������5Xq8qCl�OkC��m��"6�ஔ=���Q�&�m�0q�ܙ���3u�!1K�g;�� �	�v:���m�8��֪S����)�����o�"�ͲM�*�-���&M�۾�Xp]��X�*�LV�bpǑ����6i����݂����t�΄�h%��i�v{ͼ����%��|��~��)R�����D)�0�]��8�˚��h�ۣ�ܢ�EG��R޿�ja������n���d���f�R�Um������Z:Q��̍����������z���J�3%W�nڽ�^ey���%�Eg4�;{���џ'��]w�ݽ���Om�>�o�k�.�A��
�q)��!��֜����H�sI�6���fo龠^��=��9������D݆�ܾ]Q�2���b�{��wm����+k�;2N�fluH�ޒS�_-9W���4�}����.�������m�GG^���z�D��q],�w����B7��q�sU|w�.��N��Π����m������Uc"n�z�A����*�p-m�Kv�܋���y��g�F���!�7�Yk�{ֶ�Z�k�^kN�*�w��'n��}���-�z��|�ߝ��Ҟ_��X����/}�z<�ohۄ���k7�͚vs�fB�tϔ��_�\�iR��sޕg��	w�������I�#��_�q��o�ȝU_5l�O��c�ޡ7I����^ݶ2�D�H��)�WL���(�x!�K�l���sxU�Q��v?�ԼRV���Q����/���X�w!�g�&,�?oE�=��{��~����.�4Cg��#F��ħ�����&�i�[��{s����|߱�i'>�m35��I�;=L��w��u����-ro�����v��J:0�艙��
^zss]�?��|����]�Ƕ7�;9C�$`����~;�f�M߮;�xt6j�$,�{lQ��Ǐ��`Q�jY�jA�Q���37�������	�����=?Sp�¡�����<䫟�_�`i��ʬ����t�K��|O��&�u�<ϼ�`�z������kƍ����X��m'����a*���[�+�����-n��ĕ_3���;����Hy��Q��h[ 벎�!U"��jh�eT}���?za�тN5�N[P����+!C��&����T��T}�}UFv�M�]v�J���?�brY�1�G�iy�����A��|�����Ӷ}���b��c�>z���q�t��k��z6���$}H��k�B��]"�9���N|�P�ۃ�oo�9~��{!�
*o�i*�ݯL)�((Ż���Quj��>��p�~�k�ˎ�t���(Ϸ͜{�nu+ne�fJ�o|9�@���ś���8���g�ؑ�1�B���E���^��5%0�S������__[^9����8��}5`oFQǭC���d����N�-�0���_��}1�寽F��Z�� �i�b��a3cK��/��~_�z�0C��e�i����[��|��<����2��NwB��oL�¬�%�-���T�J����wS{
N�`�e�*��A���1{ްy��^2e��Z�����/fK�o+}񽓇"<�����t�(�2%�q����9��g��ui�L����Nyuu��~��t�V��O���d�Z�m_�:��v%��ʏ��ܵwo����ߖnz=���i��M�V��t]]����=���y��<uWَ���z�;�F��r���c�bҮYv�|ZWd��4������oވN8�ɬ�=��|�����E�ڢ�*�̏_�\���Ӳ[{wmш�_�q`��ӆֳ"�^w~�-�/����x���VGgI*ذB��^o_������sy�n��s��+�Z�?���v���ؙ��u�R�#aj��o^.�U��/��ꢹ�+sǴۻ���6�/����ԍiu}�ܔ^]�-C�Y�cg|>��_�´+%��6��n��O�|����%~u�_\8�Q���O�m�;ֵM͘ܚΛ���[�3N~��F�'�9�7^�ujiP�����6��X:k�k�����h��ݤo��m<2)U�s�@�D���i�/vl*�h�v/]r8��xm��5ڴ.���,*���ٔ	����;�V{�!n���z�!�{zW��ˣ��;�ZW&N�lǚ�E�5s��?�o����ܶ�;}�)�W�ͧ�IY���#��	����e�mo��'�����������}���o{�­��f�����������c����13S�3C��������s���G�s�\�fIV���ի���]��h��C�h��l��*s�4�1bA�}&��{��Yf�� %I�k�h��U[=d�F���#��IӝJa�Ę��n���|!](�`/as��#�,��.��?wӶ���g�u�e�{�L<��~������g�K��^l�|�3 ��-Ү�m��4��+^���w��YH�"؆ɸ���:8ANӽ��@�G��
��!�¥�I�gA �Y.]�C1��l�O;YZ�rR}m4��-
^]Q�>o�ʹt�[e��XǄ���(��� (|B�ں�Z_�N���&��c��|y�*��>�-OkI�7�cL^����2 ۛ����X��������oKT���z<l�[��B"�81�k�|������w��b&����݄�FǛ�|vn6 �8�����Ȱ��z����eG@�ɿz�=�M�p_:#���dzi�F1��P�T���� 2m��^�׀��T���쾣��^ ��b��9��gh� 2������n�~���:�ji���jn�4d�X����Y0"�?�廄�W*���=��X୘z���(��0e��T�	���HG��ڿUW�F��Q] ��A��(��\��ý鏉���
~0�^l���RY��3[��x��������Vl�:���Ѭ����pn�y�Q�L
VJ�`��u��!+�d�҉hl�z�)KJ#����
����R��ٙ:�*�PN)�	¨nf�.�6d���%�6��3OY���+�g�5��{O�\�թ�ͽ= g@�x��.1i5�޼�e�D�ptdu����&�����/�ރ�����&���u^;�[��h$�,�}���R;�TͲ�w9�+��KXy3S
�z'5�	����ii��F����AYV���}���񰦹��#��)���U랰�+޻�pH\+D�j4G�Ӹ�<n�X���i������)�iO����`���w����4X�j �����Na�OI�Gm�F��2.ujݾ�m��b.c�{���-��%|�V�_��x�Kh#��]�AE�d<s	T���$�Y�SBo�!wA�)Ô=>5?��D�C���6L�a9~��b�.	G[��0�MM���UY�-x���ɀ�}��x������~����D��)�%w2�w
�%�־��~�$�@Д��$�\�J&�������e ��{����� �9R��C��hlg�;�'�Ǟg��V�>������=ӆͫ�������M�5�Mo�E��#:#� �J����mȱ��6��f:f���jg��҆���g�K�R.	�j8�b]l�Q�(�K	�d�����}/��˅�Di�),)�2��}?��{��UJ�8^,�Dd NoT��2����(��#��7�BJ��'SD�Y��Q�mM������Ȅ��&�f��������������M:ѻ2c��w���1<�N$-i�6g��Gnk�@��>�[��Ƽ�]��&�@������}I��%֎��	Y%�b��d38��RK#���'�����[�;�Q?u����~�k�G������Wpc�5���[��}�}�ف�~M�V`�DH(���x��b��u���P�6n�����e�p_��Y��a�;Z�ka�	K�H�Lg摈��4�9W?���HM���1�C����
b��]i�&�FqD7�-���gI�e��*+�l��ņ�%�ڕ�f¤�� aua���x�'�;b�ZНݬX?9����>5�Pd.	���4�uv�B����A^VM%�|��y^g��1*Me���O�3$���|�m��j"΋�]��\)�
t��s����֪V��T<�ݴF�U�+�:�,��+T��u�
&�6Q����h�"���q��nU@8��L�]\�%qc�F��z�KF�D���t��')SYi^�Ww�m��St�X�4��O8�Qƚ�C|2�"x|�'�Ȋ��*Y�\dE�i�<�2����>f�{V����}�!%�5�J	��g<9���I�b�&�����u�P�����	6U �O�w����4���&?79��<r�耸J}v4�nUגn�3��"@�P7�TRy�<��7w�b�SrE�1\	��6e�0#�67��7�$^>������{0>��ȅ}����v������[D���ٙ�#����s*Fcz
II�R�[�V���u���V��m�C���;��]��XZ�ƙ���.�n��+|���ɽ�G����L�-|��{d�zjE�����:�C��G��p8�oc���r�,�Ӄb	*y�D����G�j�0���F�&S}3L�qpJ�<�n��߅1+��a2p��N�~�8���"����֐��ev��4Lv��UA����<'�R��7����� �b�&5�L�r�
��fI(�n?�jJ��(�{��:�~��X��!M[XS]/�D��t-��Y�=�����v=p��K��o��bz�;���25�{�S���{��ǀ+7�;l��a���r縏51Q�2k%)���Wt��􅬩,&
_���X�H7�㓼���)V,�ԏ�%��C���.7�uX��w�Af7��+R��,���6�˅_�
T��0�_U~F)|n�e2�tz��3x�g���V�䜃\ΛIf�G���t�9Y��
�&�
��j��q�a��a~�
"m�����`*[�Ws��l�
8���}��b �|u:��ҟ5��m�BA�9��s�����b`���\ �s�<��U��`Ak*f�y%H�u���C��Lґ�~>�,E�p�xk0T��+xz��p,���*xfg8��oO-&Jn��sK�4��@! �v�v�i�=)����B���<J ��kC��T�5��
��THm(�NWŖ�9��VWg���
nΒw,OIT-�:ȓ���T��e�s�Qk�H���W�8�+7/��=U�55��6����Y�~�L�X�!�Yj�6R9�\g�U:zY�E����Fp+Rl��ElpF��vn��F@Wc��I������'�PC�ke3��x�ZJ�{i��{� g�	0k)K	��8�5ff�Qgқ}�U�klOj~�)3Z�0�+TH{�H���;W��U�(O���A���P�X^���ܓ�D�Y["&4�3VD�l�XaF���m nrV_k���Aτ�i����@���ƣ5�� �='A7��%v]�M�Z�:��HuF�8'r�iq�X���?����z|!��FҍzŠ��'^U� ������I �O�R��kٚjB��zc[�?<M���ɷ��|��Q���&�U��WU�&d��Rb��m;�����Y=>bt^��m<c������I�`�����+W{�����+�ā��G,�MU$�����`��J���M<p��(�6��N����Nh�D�o?z� �F� �U���[d�اb-�`ԝ���c����� >�4u�:׍�O��rf��\O��/wc����f�~����Z}'\^7>��,����W�����N�$[�����g��
��"/-!�DɈ^=�94[̦OҠ�&�����H����B�T7
��t�d������A:5g]Aٍ��o[)�~����ZU'<=�Z��j粎0��{��5�� >JNZ��/�����Z� �Y��D��M)+�@��:��n1���=܀��,��_���������x	�Z�N�J��~���Tr駼�V�-�
����R�N�l$ߍ���C�)<U��⪃X�8�3�%�F����Ѱ�J�:�be�@�ޅAV�<�O�	��}�f/#�\n#ѕ�6k����c�Qfr�m'���q� ��ۚ�7m������a}������	[�L���D6����0�������җ�(*�0�}���E�֏,����#DB�|�	�m��	�lh���-��A��gx�������6t^)��`�<د+.�w��\�0�*�\g�V,EJ�G�`�{b�I����y3�ꀂ�!<�&�`��G/�Z4�x}��;P�0so��7�1����\��8� yȡi��������x/��w~q�4�����Y�����<�)���9��.͘���k��W�����A��e��>)xWt��g���d+^F|�Y�v�u��+���$��rՇ�˽��ۡT�#ఋ7���aH�I���'Qb��y���b�Bѹ�i���G��ξvAK�W�8�ym�^�<���""��@���p��qY����
�v�G:�� FNJ��'a��V�f��I�o��9��Dc�LS��9�����:uor1�t�!դ��=��u��󞨔u�h������?�_���n��{k̭5#�N��h6�/ �)��b��k��T�$�JЌ�(��;�P[���?�l�K)�#����
����]I�`�i(pwve���m�J������(x���Gb��'U�z�����JCי8�q�Z���RJ�P�#`ʮf^�(7oݦd��;ɲ\�6�K��-~(^E2�"D���I�}R��#�Ƒ
ik��5)$�̧�|�:zjs�1��J�OB�c�4��/�U{ԙ��^��j��!e��_�-��S\��T��C��c"(*�g�@ٵ�{\�6�5�E�}] \�Sޚ���C.�ȇ�݋C}�&�',9I��w4�M����BqFI��u���~�ʼ%��^q�V25\���'m«\~B�no�0M����u��S�+V�Y����eh���x���w������b��R�U%<U(��P���+I�����������yf���}�Z	��\߄�}�;��:�H�:�2	Df),���6{�l��XYC��ڠ�O�� N#b���gމX��!O$Ҽ�,��r�����Cԋ�k��ש��$���/�3J�\J~�H��J��&d|����κ#P�0[n��]�W���Uэ�ە�����8�t��N�.y�\�\�B��~�o	�M�1F�e� �T��B�J�}�Кx{ڝ�Խq|��.t�gL�H��Bs�ݥY�������+m�(�QE�z/�Y�c��/s�:G�7N�q_˖w�^T	١�,,���n��]*�~@�M,��y�`�<�Kȡ����VV�؅����7�ͭg�B�VM���URR���9IZ�,���O��΃/��.4j��e�������H� 5C��Q�n{n�|[jO�T��.9&(��^N�{(G�Kڤ\ߎ�iߖftr�7j�;�d>i%|m��H�	%��	����`Dǭ�i;(?�����tL���� �$��O�L��u6Q���e�I�u�+�U��u���s@1t
�bWRb��s"���V��.>����У၎2�i�ڬ���D/=����j`C��lE�^�CowS}�
=��Yr�2�G2�(��C��^����w=�gtI�棄x��X xw&nC��c��l��Q�b'�=axѤe�[y|����̢���[�����;*-����*2�%}������<��������y1�F>�*�c�2�$�A9\���}c�w�
�%i�聖�V������]p9�����U��t���5
�S�S�<d�^�6c9����c.-ځu�����BØ՞��Q?��/G�3��\�,�ؗԚ�l=�K,�ޭ�z�S���UYui�����etVgS�����������o���������������0���������w�o�����_���o�����_���o�����_���o�����_���o�����_������J�=3�I���Ǳ�$�0H��H����K[���
�3v�������4��)�=+������2��P����DƓ�?�����{0u�R����Z%g���mmUס��������η��m��mU�]� ɩAl�]�h��8�죾*G�����۰cTw�Oߏ�ť��R�ٛA�}.�F�����l&`b�쭦�e1'��?^��m����~�V�~��ݹ�gI�R��҂';da�b���a����������Ƥ���b*;�&�ꩨ�P�b/@
="��Q���]�%��4E�.
Bǔ�{
���<N����vÍ���3���d9鮺�U�������^.ÇT�{n��&��(�%�F���:zy��e NDLԯ���1�^y"��J(E��"ɌK�J-�u
<� -Ib�M^��������Ii�s=��HD�������ӊ�u�L�����`�G�i�N�MLέ��
�c?���]�N���?��{�CY���՜�lz<���5�G�����㺈���>Pq�_�YA�_S�/���C2�ao4��ZF�U���4]��B낲�0��@^�M�ۀ����"�!�� Pz�3�fZg`��^��lo���u�pE��]T��.��0Sy�x�<<<���Q�죦��4��nuײu���;�L��XT�ah��QtMz�q�QGo��C�u�#�vJ���g4���R���
�
KDS�:�K;U����DiRt�1c��5eD���O�b䘸�(8U�w��ʹrW�\׌2�rz4�lx�mIH�J�P6}3S+�
�olɞ�r���B�W_y!���i(��V��W�ST�A�lL§2�'�?��L�%��`F*�)S��<I��q���h���� �9o
=�
��:=_di`�j@.p��uKW�n���QY�c}G������"4�����'5�_���D��Nγ��!��9�1q06���lkQ��^��U�8��_��%�%5���8���� �����|7p��_{t}�,���_�Z�\���v�l/�pE��w�s��U����$����đ4yg�x��l��Jʟ�&�
��g��z4�mĦ��h�Y�$������׉�dt�ˬ.]�m'��
䩭moc�"y���
l�Z^����2���n�f���P]x|B8���Q�쫁Q����t�Y��;��dm�\!���F��ٻ������܋��N�~^�.G�%�a�V�ŏ�Kf�>֛ϟ��E.��YyvM"ʥ*�Ƶf��Y�wO�+�����"m��_�~���<��д�`G�Y^j��*��)�>""�|������<y�Ol�����6>�Iy��ho����K����^�_|�@	�>��r7	4��衇6~iT�:��ja4,&�*%��xb ֪��H��|���]�[b���X��,a5�M7�9�G71���+Ø˘ �&W� �9�oV�ꇟDw���!�)�	qP8�'�o����mA�mɱ�p�k�l�1�H����i|����]���E�Z�p�Y!�ԣ�����d �����2�C�x�V�rc��	��#���;gZ�'q�:���R���K�M����4}&_ډ��~�_��)�O�'V���F�d�y����)���*�*�!����Q�l�=ˠ��rz��S�(0}�y� *�96~�q�[�=O}�׀���UdgM�v��A��]�"�#�/�f��D��&M|IN��� ��*��:��� &Y�&t�����1�;9�{0�mD��m�i�e�LFhV5e^�#�~4���ع���o�l
	dHS��M��G��A������^��ǂ��a��4,@�?�7F>!��JcA��7E�l�C�e,��9o��<V!	�B]a�$��}��D#s���!��E
Ջ�
2ד�*���P+�fMHκ��otZJ�SN�R���q�*ڳ"�&�4�)Wv��p1er�<�����������6S�Y�Csu�<�DJ���9�d�ry-jqڎ�L�2إ��R3�!*�6h%{�MNK��.��"Nݹ�O
�g8�����E��� �V�ӕ�7[Q�.
�������h͆U��|����ٺ��%��V� �W��O����݃��/����1�`�a]t�9x�\����2���wJ״ ��I%"�N��>���K��U�z�Ӈt��aM�!�ww論
M�O����N��P���K�bK�׍��5������'S�ȇOG�����,�.�sw��	�L���E�>A���ɸ�ʋ�`��gպ�b����@K����0'�aQ��R�hr�/x���9V	�3��!d�F��u+�������NfŬf���N���S�F�v�"FK�J���5��2qΨ���H����2o�<���;����OyaF&�:e(ϫ���MhQ3�L��=�z@��%��B�:	*�9�v$�9���B�^l<��2`�V�5����0K�M�K�O3A�I�#���*�g�vC}4V_�DUJ��0�4}���B͚R��M鍿o+�E 6`�7��X�REI0�<J�JE�}"�j:3w��R5���zXEaJ�o�П�!� ���i{s�0�L~�c�K�4>�$h wNN���˨�u�K��J�!��3� ����|���٨^�(��Q�8)]0����&��-��vQd�4h!BxU'�(�<OPA@S��,*`��]I��Qf�ᯄę�����2�{�����P��P)�g��8��[���=�ܧ����SF��޳�t%�*��B~K6f�eH��(��9bD*�{��-#g\0��{i'՟?���;]�D��Fz�iS�%�ƿp^���F�F;׃��:�<"���cB>�"�`��̨��,X�mԡ�)��>�J<Z���!�:j�k3�ZW�[�8��W��ݵ1�9�-�X�� ]~/�xmhX����zy��|�W�UP�����ϭ��tp�y�u�$n�;g��K��%�.��}`hd%N�܃��I^��ͷơjZ]�U8^�}�mq��Z:��q<���G<�
�X�.ZN[O�\�<Rr`���/�t��*���'1�!G� c�S�e���j�s}�X�qK��K8QZh���w�p�����<�%�܂�T��S*M~"8��_���x\=��lo&�bQ��a��Tl$��40�����m�{XJ"u+��5�Y/'�7@6�My5F�Bf5Q��FiV9�����$G܄@���#�#�)���G����s=��O%0y'�-Ag疛�
Bk�9���M_4d�5���%�4^!��c 7{�4:yg�Z��[º��R҈.r!��o|�k�Ut�o�F��\"/t5����D4z�h�-����*VrgՉ��B5�jciz0��]J����� ���O��!;��?T���:��p:F�3u��<�	q���v���K��i�	!�?*�Ivp2W���zX�B�b]�&���B�!8������4�3Y�kM�NǸՠ�]|�,o��8S�2�>iSח�����/������ �>�� f;lՌ2]q��6��AC�| ��>�}a��O��0��#�-��y�D5o��l�e7�LϘ��X�ݘh������(��;��Y���}���:p�+Y�[<�znOy�B>'�=�z��[��9�d$��n85�� ��%��[_�|��[V����>���~Js!�x&�k��W��q��k-�\E�����h����$�?�טa�_���������I��A����'0�&�p��Ӹ\��cPdD��v�B�"�ja���!�L{�&��E�V�l|g}�5f�� �A��q�Lo�-�I_AΓ�-���1��>�	�"G��WՕ$K1���]����1X<�(�$J� =��Y5aʠa�o�u��z{�E{�I,&t`�M�k���M\U#�n��$�Ǒ%�.w�F�NO��G���yDQ���%	̳\~���N����/�u�/���1̞OR7��ʕ�><z��>�;f��V6m\���#�{��KO���e?V�hz������o$DZ^4��~4�<
�X�Iá��V'�de%�)�kYg�g��+2����! FR�6�m���=��О���Vx�c+����B_���E4H�x&$��nSN��h=�4�y��5��m�2+|D�\4f�٢L�9�\����᭻8��mu8#��@����Ӎ�Z��dd�H��8�6�������k�x��_�"9�s�F����d��6��.�+�x�1m�>��U���cH�HݷD̮�!��4�9Y~Y�|Gsq����[(�����Rn�Ϩ�,� �D�b~������n3��C����R�-���A7����}>ܑ����$����-�♞BN>P9v�rS��\��j�K��R�{o'�]j�V՛1����F&�ٍZ(3G,�0�;2��ו�T�CA���cg���t���0m�;�3��b ��F�A����jz�b��zI{x�hH��{I<c�^��e���f��0��u�SI�ͼj?Ya�"�v����-��y����0сϰ�f��'ѷ�6���t�I��,#�T��Ȥ%����U?�g,~F�'�-7N�OC�IѰ;��d͞9J�-	�E���%�L8�e���ҝ�������Ø�a�+����Jj�����Β ���/��q�����UQ~�����s)N��G��n���ץ�c���涶�@ճ)�`����O��U�g�&�t+�џ�c�Dc��z�is��"Gg%��0[��,�[v�F�i�P�{�e��#�3�M�Di1���'27��~���%8E��5�[q�	���)�[�Ym�H�w�S�����/�����^Z�:7X˳Qߒ�a�i�`_5//	��y��dQ�4׈A}��>���c����~�6����~�s�2ې-��D@��H�Kr �_L�ɸ'�h�.��jLȚ�Bk��.�����YQ�,��@�;��U,�?ц��P|(��5�L�D.(���j��{/��P��ICl∓�B���ve�o�r[GN�`�3]+��C�=�R�M����\�s���piQ�C����]�Ȑ�ov����B`�� �)7�BgD|�x�?6��/>6�&J_����6T�>>\6���r�z��o��=���Cϭ� �ؾ-�}���?2=+�3��ْX��Ƅ��?i���y2��o�~�Xx� c�i�"�Af��80�)_m	ՔE8v��-��?|�u'�J0�<6�q��I�,^��h���O�MB�����I�Bj�U�*C��*cF�H�4���"��!�d���9�*��x�b4����e��XJ_b��E"6Zd^��d���q
��n����� )���$����"��]#G����am=�>�m$����s4��-�l����"Q`��ŵk�פ�ۂ흕)	����ͅӺFD��dBZI+1���n���JL�D2��,%/o]�����EZ�T_��{��ܜ���A��6?�-T������R ��{��T�ژ
"�<,�Ay�ycI���L�V��2�LI8��"3��Ma���U5Ey�&�`��S���9��{���Zx2��/zlLS�V�f!�F�+�]�w$�W�K��<A�*�_b#�V����}>0�
�ĭ,�����o�uVD��R󽧌�K܈�O�s}�%���	XI+0fu++;������/�^�?:�d�$ �ە��nt���|�r}Z*Be��s��%$����X����"�F��e���o���0�8��n! �KE�Ɨ �104p)UY"̈́��q.w~;݈�)�%�E.U�Z��ra�Zc�I��I�ށ��dr	>]��[w�2�^b������W�]���Ƅ�7���D�,)t�IMJ���$$��ͬʴ\�j_�1u�:�v���;t����7��o�W_fOY��Xߤ���uV�<�����b¤�tFa@ �V�6C�fmf\��a�ٟ΢�m�0��յ�9yItҷ2��ꅫz}�s����?ʿxO�J�زq,2�0N+N^Þ� Y�:d������� �����tI 	�Z--2�{���F������uEC�fK��s���N:EG/�v��u��nt����R(�e�ʢ�
�K�Cu�=��NO�S�ษxs�>-�-�[�|J��`�$F�h,VtI��:A�0|�Bo��ۇfO�h��δ��@�p;��{��m�q����=J	7�ڵ���x=�� �h
I��a��ǌbX�L|�v	X��I��a�fP$*��<�:]�2�����(~+�uʕomS�3G}D�J.J�ORj	�̥ ?���i�����;��G��J��N�|G�k��b���h�P�ʥ ־1���m�O�'!:��|����w��M��w���Ko�ܗ{�&x�f�R���\3�׮��؂U�|{��\C��{sGS�ڲ�������5�Ԏڊ߽�9�~\M+��mB�M:��������i�]d��+�V���᪶�*g� ������s<"�k&���I��Ci��@;p���i�)}���{�Vf}���EU�j����!�<b�A��=��JN�;;\���)�9���O����<��~..��kev?��#��f����,4t$7�v��Y��ܭ���vk� c�jc�(u�5�Y��@��D�Ӿ���Pn�U�L��3�9��!�1� /v�t4l�wBO����h�4�x�{e��,��'�;[�V��f5�l1me��aH4+Y�SG�p��f�����12	�I�eP0�Tb��*�σIoT�y�r�ْ��[:ݳ�΀��m�T�g}�<b�ɺ�mA��^c�NG�6*þ��^&��>d��&� ꂖ�9�*�괎VFe�h{?/d����%aˬ��~x�x����+Y�l��R��^;���أC���ד�%�ǂ�閊:ci�R��q��H�P�&�6�o��;�J��v������9�Y��&�9�T.���GBv���]���YEi�d�[N���g�4�T2��#������:$���hcfigr��>eQ�Ol���vK�7K���7�y�?^=����~O{���s�
(�+FA.`�R�j����[?�ZñYQ>�^�`xr�h]]]�4��1�%!���� Ȇ�G�P)'�yx�L�_��B�ҽ�f���S������ՓR��cf����� KyH�7/�d(���0A�Cd�=�r�`�j�c�d����l���׽-�xy7�'�{`y������&���
m�y�yU�t6�XB����O�4+��th.���lg ����v�^�qJ �JB���s�]v�ϼ���b�Y�����+�vp�.,��Oܮ�I�<c �+Z��;�U�Ő"��m@R�%<�.1�����-*J�_��];�1�#<��1�A�A�X�Թm,r���(�>Ȅ��0��Z�g�	C�7S(��$<`�j��>�ɒ�k'�2�-'$#�(b�T#�fuz.��RD�I��s��(%q5���\�\h�~5�x�3�l肗�QQ� R�8Dw�=�⨇<��n�(<����~Z��HH�+!Y�4VW�\GUH����.4��0�{�>�V����C����'%�o���g�A5��X����:T?x��0�U�j�1@�����k�V!�^��DV�C>,Q�m��Mw<��@u�D�[��uѳ�!�D;
�[�!7��zB�:�M�'ݩ�Eqk;P�U}\w�<�U��S��D�4iG���5R"M��A=��A��vR��Z�xS|HorU6:�X͚"Y��ڢ<mx����hd�e0����89��2���	��F��P���kzmIB�E�h>���T�"9�e�Ҽf�Ф��,���b�[��-a�'���0Hc�GmZ�5�է��^���L�*���U0S3W`��� ���XE�c��A�$�8�l��z(ld�ԕ4��0>�v=<$�WL�Ԕ8�	� P��:O�i$�aT�r�}/z��>��Ė��4���i*-�)I����Y��i	Dj�_=Ov|g�]�	״wmB�5k�9�؛��[P�d�j|=SC�mӺ�1$p��y��m�#-H�V�7At����ߤ�E�K��`�k�b��HI�3&:�f�J_�X�(}�Y�r���s�BF7����2�,���d�ryDeDվR>(���jS3V�+5
���XLD�`�zKfn�gU��z�|���K�z�w�+b�;yZN*df�ڱ*��A�f��|�D�ﱷ�8�k���ea(����_!%(����u��̎�s�>�P�)�H�!�SfbT<c��l��=+����>�oӆ�ϫ�)P�;��ﻙx��Ѓ�¤r!��^x*�z� �y=����9� ��gL����j�������O��6������_������-��������o���������o���������o���������o���������o����?���F��K�0����}�����0�
�7yZC�$��s#����;��j2u�����
��� %h&
�p��Nh����+d�d=}�2��<���-,L��>���&WW����&��-��Lށ� ��S��qq�a~Wέ^�=s��JwrE[/�=��� ��C o�ު�����=��{�mO�����j����-d.��k՗�wW%�յ������c��f�._�Պ�k���� ���ı[?�Ǖ�I������_Y}��Fg��O�@f���~�ƈ'}B���/�Bz��ǔo���',�Z�*l]i�-Z]#��"�H�!$y��qe5��\��v����6(�{ʰ��"dol�]������KS[o���Q�4�ޱ1i��ٱ!��x�TKa̕`Z�V��$��O����Rs�Z@�^���1�^�E����L�[i�c�$E}0�h\��S�v�+O����:�S�(_�w�v����=�%�����Mn�kAi,���o��\A �Vj�L-��Nʛ7���� \����0�B4_�����}E�sp�B��S��eM\��abgdE3��:�X����P&W�=�lBq-���Π/��e��懬��_T�C��A�>"�@mz4X&E��'>�)3Ob>ZUOmՍ�*Z��K�H�f]9��W��b�9�hj�&pf=�_lݵ0�Σ?]9M0����X��ߐv>�N��Ox�Mr�C�%��S��C�#����?��|WOU_zp�W˘��*a�'CUD8g��+���5��������A���>ǡn��~A,O�ZP��Rk���pb�FF��Ӷ�<$�|�}�]缅a��,��L0��Zj*��c	�P���(G��O ?��0ya[�IHSAC-�f��R3������(�H��I'�<c��<0���zH1=�3��:�"ˀD@p�y�FӤ����O�t3��R��R�E �a��*Sjel�����q"E�`�T�gc���qM�%�Y,b�忲�������,�JQ:���P����h����%j6z�=�ʺ��t���<6��*ϑ 	�N��B���s9aɦ(8��(E�ټ�˳�B�Ӈ��Xy�
~x�,ȍ���܆��ty 1D;2>�� ��p@������@���J�}�y:�q�\�/��B��]C�כN��L�{Z��d^���������~N���D��~���~���I���'�Pbttڥ
��v��\`�Ls���j�TK��3c����&�z���չ�6���z?�#�ԥ�6�d	w"�Tn�%M����Q����0ٓ�
`Qzm�=�8E��*^�y��Ʉ��|t�L�t��/O7o��թ ����u�/F���+ ?kǄ�-A��m<1���$yu�SAvK&������8d�:�_�}��4W���^�@>����8�2,X�͍ Htk�q˗���օ�M2?9?����k;( ���?9ôQRP��Q	�ZvI���.�&�	�e-�D��d������&#�O�<�#M�<�KpB��e[��LBn��8B�>�L�\�p�aB���Y��K���>���|�z�8z��#�Լq0�0�˗��ys�T��o�qLH��ռ^r �����j�/=�
�C����m�9b�M�{�s-*�?�|�$���CN];35���:�ј�{@����/�I�:񰓠U'�?X�%,*oKH)��:It�F��㷄rb���Pq�t^�$��ϛT�J�.�hI�O�����Be��!��&�I�P^2����<��UK�)�ߪ��"D2��Ǯ��Yu
,Q������N��$�@��վzQ�+rA�'�y`��-i��m��E?)��_��<r������P-�"� �4�{�o~"Q��b�g�`���E昬'i�jj�,��Z�\�LY.�$+Hd��X�?>`��7���2�P%���+���zq��c�|uzX���>���@����^�>�'�d�i�ZH��eZ���6u��ҷ,�
��K;����0������ R����/�%�5���]�p�	8������6��+�η�¹���+M�XoQp��Z2��o�&Ǝ�����C��+<U�\��d�<t]���l��!�b��#�ʹ�I͆��-@�@p?p��k%+���~$�g7�T`�U�=Px��^��I�f�ӟL"��g�K�*��0�3:#�	g��u:��:G+`kpr��Lӕ�������g��?.��ʏ;
c<���w�sM��5�/]�
���8��Fy ����ŗ��#������J�%�w�
�7��ݾ��w�_�Sߙri��O��M���8�u_����z���t0*.✫�M`��O�_���̛+��G<ق�����2�<�A����������i�bA�ڿanA_R��}9u&�� ,5�����r||�e�����K�y�؏�0|ڼ`����U��qc�΄vhU`I�}�1n�D;t�{��aB��;CTFk\�0FO3::"Bf���Md�EQ�OFLC�QITm�O�_���O�I>��8��g�X�zϴ h,����~����A��'C��������@詽�wt����V�����sw��<]q~8�|?��1Q�T<��[�ў�l#깚%W��?����Ji�qRßګ���f�m�J`B�5��<��H�~�I��¢��>���g Uk�0¯���q�A�eC9����f��_�M�퐉�����Lti�W�7�$sr�@�^�e+N��|Q�":19�!x��������u�Jf�75�ܑk,,s9��������(�b9C\Z�v��ސS��C�����r��9*熺yv���N�6��T��{s���_�sE-lSha '��~*�`��E����ke�yt�<7��C�_#��7���?���#�^ݹ�<��w�����S}�=&����{��Țc����	H"Y9�R�N�9��L�/��M�x�1����A�] ��K��L��^6+m�r�HY�Q�Ә�oX��	�?X��P�X�(��R�v+�:�ì�]��k��lH+��l6��=�9�jrVAh�y�Z��C^��A����Ƹv��x=�a�RB��������?�nގ!Hk"C�^I�b���+ȷ<
�G�1gR�VL�4R4�.������c��ז/�xK���t�Msv��� �r�zǜ�P��Rn�:{�O��ԛ���Kض��T��^�=��8y����¢��gR�z��^�2�SJ�҇Ϡ��?���?��I�-�\��(�n��6��������3yޅ���?���?o��?W���\�s�o,�/q�'��
�z�f���́�8���S�����/��S��t����J�EL��P�Cۙ�^`#�ѯ�|��ԭG�t�Ɋ��F�dCh��g*-�R�"3�rv�L����LZ�F/�~�����|�cH���w�,�n��l� ��E�Jh�ˠp����-����O�/����D/�e�/uA&�y�	�N!O%�
��΂�I��I��"O�=po+lk	^�)�%�ɩ������8��L��*��b�̓+�Ikyj�K쉖������C�yl���!r�BL������ic	�bf�⁝�~\
1�Q�p|�=�\o�/=�}5-�?-����Fg�p"&��O���a�N�_�S��3(�XH\Hs�c�x��%�X3�R��,������7fr<H95�RV�.9��H�$���Q�f(�El�^5F����:�qrL�����.��/��!m�Va�- ������	��@�������h�o��k�*�dK[��&Kߌ;�S�z�&p�ұ���)�8&>�+_GZ�V�Dgx�D���\�K�~X#s���~�����5~���{�M�\'��<��u]l�l����x%3��4Y	��?���2�>�h��X x{��3H��B�=��H�*Y����Q�	�C	�#-.^��&�� �l� �itLg4QH�������2/� ��8l���tw�H^�׭ P����)%Z�YT�D�k赫0� b���3	�M�S:p���V"�8��<��%�4M&?O7��O���犊���s�\�!�O�d�׎W�T��4k�E���l�Ц!��Ϧ)6xwM�X�X��Q6�v�z��u��Q=t�7��"yh��Q{k��l��ȑ�+�8&'�a�4�3a�����_����*����n&C��A7��?��v�q�@�T����Z��S]�.�S�Y�j7g� �O���ez8W�^��$��sX�y�IGL����k���/�s-*9k�;�:��3H��X��@-kI����>7�-ѾX�\)�"_���1[��+��׏���}~��Ϲ /��G��5o�Ŋ�_���'�Jʟ����Ӷ�c����M�p2lE��?��VD~oa"���#G��߹{aD!({*_�(����jO�
��q����<Da�����\궚Q���l��h_=\��=� �{@���u�����:�4�m�\V���i_E�zs�ߌrW ��:k��#���?��܌�9n��#2�8H��;Ke�=��7�覱�ۯo�Y�H�J�:���M����'�-��C�j�$} lv�
!rT�HW�羦b���T3����r-C.C�U�IjkO%�殂�j6WC��J[E��2_{Z�<=C!k���'�r��N��_1X&������?�3�nt��҈�s$a��I&�~!���Gw���)��.�s���\�TLh(���ҟ�����(���ڰ����_��
P/.�e�ĳ��7��A�+�O=o�~e5	r�^��Y�.��P�T�K�>�v�G���C�>�f)Ƞ�`�$~c䤈�L�(�*����v�b�\���c<j��-$�V�\�XkȄ��9�9��^�p8�����_�p����^�D� ��'-OR�$�m1��)1��y���TB���NX=����'��'YAk��. �P�W*�b��NzE�H�;M��2r"��e��F��x��2n�`�Jj�<�s֢���Q`86������ ��(��3�U�#���Ha~L����r:��:r=?5�;�#J*N�r�~�9.\���.̔��$�k��B������lȽx��\�ܪZ:v��a3�,7){Ok\ryy=͵r���C�7�q�6Y}V	ً��oƼ�ϥ
X��II��O�|�)n8c��??K�H\�%�[oZ_��b�>��k��_F'�6�f��s��F9��"���Ot���4��P4j�<P/:������CVm�����>���R
�����Z�$8g@�y|kn"�bɾx��O/wG�V}r�f��X�1CE�k����j�2Eb��!�����$���L����� ���;�[��%d��h����/�_��I�"������{Uk�OH�L����8�a5kC������ܕ ���	pnĿ&fu��b$�jB���1g���R=N"��9A^����
�bt|@����l9��?3�c��1�>�:pNs}eRb^�|Ɓ�
�I/�bȰ�Y�bM�G�%�J2�����8 N�e֑�|��b���G�y�C�y8����d(3p��M�D u�P��t	��GT����2V��t�������Hew���Ka6�٠W]��݊�+����Q�K(�$������&��k�x	�k�{�,�.�t�����&���i&s� �G�N��-��<�KE�C�;��9���5C?�!�׵8F�-9\���D9�@�[����G�\9��Rs����X0�;޼����9�gp�V�/���Ge-NݣM-��ȷp�&�ֽ?��hh�:-��X�X�������R�i�<���'hx��9�Y�S�j�^�Y��һ:#��J�ɷF ��+ᦨU����~b�2^?����;�S���DU��{�b���i�*�a7�# @��)��MϞj��E��?��V�C��>�.�?b����O�gW�'�F�{Ϋ���|l��rh�'��:ؚF�!��)yS��R����~<8oma�:�fh��ۚ����o&ܔ�B�Q/Kl�	�rف�^'�4t�=ML��)���wa�����wDts[O�h�i-�!ܟ��RP�F���d��_n�B�7�{B�z܄�'�X��B2�k�~�75��O$	��dN��]V�h��]����e�f�<<�"/�F��|}��p���h<��˥�t��:�G��f��5u9��Q2��PB�4���,�Ov�v�,�䣥���$�V�a�:�;3�ѻ->�\H,d�r�ʐR9�p!Z6EI��(��w�ؽ�[.ʕr�W���
�@p�ؐ@y8݃��栟�d��+,���y�+p�<-�M��Q����}n�R�����M��x����Y{(�bx����f<�q�j���3��CB�蘫���h�-�zT�4j�7{֮s��R7�Ŗ�.�O���� ��	����M.����,��i�<��	����H�2�����"x�ִ�f."��]�˰�:�̮�n����!,� R9�S���n��"�������ؾ�DF�U.A��mp%��o��6��N$��(�~%�ޔ�~)��=l6�l#a�o/�����ӗ���hOB`��3N7��~#��y����P��}��x��pw�ikx7�<�"�^<�"G��G+��7،|g[�.��*�T�yH�<����Q1qN\�v�6YW�kg��������i�+론�`����8��ơi�!U�P�+1��GB7�1��y/k!�L�y�5�����ڦu�k>��m�����a���sY�1Y3�V��$���T���c�S����X�X��� ơ���������rSZ������|=F_�ZA�o_�P��o��]E5�Ր�V�J�v]��q¿E�i:�{	��1���*�*�O7��A4�֜/��^�T2�P(�,���3Ȣn���p�ݿ����������02����'�������������)!=��2п��O���¯$�_�
R��G����a��<���E���K�~%O	?�9;��"`$�'�O�������?7�W�ܹ?�O&a�f�K��	+�m�,�8t&fή0�R	V��cf�f�&V&VL[f��2���?��?bV��q�{׉E��/H����̍l]��M�	��h�/T�?h�
S{�?�_��+S�W���������]���f�L�n��e��U�f���)k����3���k,���������y�iA���!��������76�����l��?4��!����_���,�?�3�.��|g�2u3�r����?��3s��c��i��=��LϿ�rBFz����2������UYa`��jQZ���z2�����Ct]�~я?smL�������	�����%���֯���}~A���y�?n~=�����'$���ݿ6�������N�Z�?��}�ß٧���O���]��w�����~���]��w�������)� �  