#!/bin/bash
cd `dirname $0`
mkdir temp
cp test/dummylib/*.h temp/
cp ./*.h temp/
