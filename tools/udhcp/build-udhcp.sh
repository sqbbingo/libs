#!/bin/sh
# filename:         build_iconv.sh
# last modified:    2019-08-16
#
# Example usage of iconv.

enable_static_libs=true	# true
enable_cross_compile=true
cross_prefix="arm-himix100-linux-"
target_ver="udhcp-0.9.8"
output_path="$(cd `dirname $0`; pwd)/$target_ver/build"


usage()
{
    printf "Usage: %s [-c [cross_prefix]] [-v [target_ver]] [-o [output_path]] [-hs]\n" "$(basename "$0")"
    printf "\n"
    printf "Options:\n"
    printf "    -c <cross_prefix>  	enable cross compile, with cross_prefix\n"
    printf "    -v <target_ver>		target_ver is lib version\n"
    printf "    -o <output_path>	output_path is build out path\n"
    printf "    -s 			enable static\n"
    printf "    -h  		print usage and exit\n"
}

fun_getopts()
{
    while getopts :c:v:o::hs option; do
        case "${option}" in
            c)
				case "${OPTARG}" in
                    "")
                        enable_cross_compile=true
                        ;;
                    *)
                        cross_prefix="${OPTARG}"
                        ;;
                esac
                ;;

            v)
                target_ver="${OPTARG}"
                ;;

            o)
                output_path="${OPTARG}"
                ;;

            h)
                usage
                exit 0
                ;;

            s)
                enable_static_libs=true
                ;;

            --)
                ;;

            \?)
                printf "Error: Invalid option: -%s\n" "${OPTARG}" >&2
                usage
                exit 1
                ;;

            :)
                printf "Error: Option -%s requires an argument\n" "${OPTARG}" >&2
                usage
                exit 1
                ;;
        esac
    done

    shift $((OPTIND - 1))

	printf "enable_static_libs=%s\n" "${enable_static_libs}"	
	printf "enable_cross_compile=%s\n" "${enable_cross_compile}"	
	printf "cross_prefix=%s\n" "${cross_prefix}"	
	printf "target_ver=%s\n" "${target_ver}"	
	printf "output_path=%s\n" "${output_path}"	
}

fun_getopts "$@"


# Cross compile cflags
if [ $enable_cross_compile = true ]; then
	cross_pri_cflags="--host=arm-linux CC=${cross_prefix}gcc CPP=${cross_prefix}cpp CXX=${cross_prefix}g++"
fi
cd $(dirname "$0")
# Fetch Sources
if [ ! -f ${target_ver}.tar.gz ]; then
	wget http://ftp.gnu.org/gnu/libiconv/${target_ver}.tar.gz -O ${target_ver}.tar.gz
	wget https://udhcp.busybox.net/source/${target_ver}.tar.gz -O ${target_ver}.tar.gz
	tar xf ${target_ver}.tar.gz
fi
cd $(tar -tf ${target_ver}.tar.gz | awk -F "/" '{print $1}' | head -n 1)/



# commit out "^\tcase INIT_SELECTING:" for compile
sed -i "s/^\tcase INIT_SELECTING:/\t\/\*case INIT_SELECTING:\*\//" dhcpc.c
# commit out Makefile install prefix and SBINDIR
sed -i "s/^prefix=\/usr/#prefix=\/usr/" Makefile 
sed -i "s/^SBINDIR=\/sbin/#SBINDIR=\sbin/" Makefile
mkdir -p $output_path/sbin $output_path/bin $output_path/share


# ./configure
pri_cflags="$cross_pri_cflags --prefix=$output_path --enable-shared --enable-static"
#sh configure $pri_cflags	# ;echo "sh configure $pri_cflags"
# make & install
make -j4 CROSS_COMPILE=${cross_prefix} 
make prefix=$output_path SBINDIR=$output_path/sbin CROSS_COMPILE=${cross_prefix} install



# Tips:

