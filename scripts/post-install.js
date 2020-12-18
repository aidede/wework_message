const copy = require("copy");
const path = require("path");

// console.log(__dirname);
copy(
  path.resolve(__dirname, "../src/*.*"),
  path.resolve(__dirname, "../build/Release"),
  (err) => {
    if (!err) {
      console.log('".so" file ininted!');
    } else {
      console.error(err);
    }
  }
);
