# set params
NDK_ROOT_LOCAL=/cygdrive/c/users/angus/android-ndk-r7b
COCOS2DX_ROOT_LOCAL=/cygdrive/c/Users/Angus/cocos2d-1.0.1-x-0.12.0/

# try to get global variable
if [ $NDK_ROOT"aaa" != "aaa" ]; then
    echo "use global definition of NDK_ROOT: $NDK_ROOT"
    NDK_ROOT_LOCAL=$NDK_ROOT
fi

if [ $COCOS2DX_ROOT"aaa" != "aaa" ]; then
    echo "use global definition of COCOS2DX_ROOT: $COCOS2DX_ROOT"
    COCOS2DX_ROOT_LOCAL=$COCOS2DX_ROOT
fi

HELLOWORLD_ROOT=$COCOS2DX_ROOT_LOCAL/HelloWorld/android

# make sure assets is exist
if [ -d $HELLOWORLD_ROOT/assets ]; then
    rm -rf $HELLOWORLD_ROOT/assets
fi

mkdir $HELLOWORLD_ROOT/assets

# copy resources
for file in $COCOS2DX_ROOT_LOCAL/HelloWorld/Resources/*
do
    if [ -d $file ]; then
        cp -rf $file $HELLOWORLD_ROOT/assets
    fi

    if [ -f $file ]; then
        cp $file $HELLOWORLD_ROOT/assets
    fi
done

# build
pushd $NDK_ROOT_LOCAL
./ndk-build -C $HELLOWORLD_ROOT $*
popd

