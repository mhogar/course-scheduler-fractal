# update directory
cd include

echo "Installing rapidJSON..."
echo "Downloading soruse and extracting..."
wget https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz -O rapidjson.tar.gz && tar -zxvf rapidjson.tar.gz
echo "Copying required files..."
pwd && cp -r rapidjson-1.1.0/include/rapidjson/* ./rapidjson
echo "Cleaning up..."
rm -rf rapidjson.tar.gz rapidjson-1.1.0/
echo "Done."

echo "Installing catch2..."
curl https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp --output catch2/catch.hpp
echo "Done."
