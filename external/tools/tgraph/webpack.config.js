const path = require('path');

const HTMLWebpackPlugin = require('html-webpack-plugin');
const MiniCSSExtractPlugin = require('mini-css-extract-plugin');

const SRC_PATH = path.resolve(__dirname, 'src');
const BUILD_PATH = path.resolve(__dirname, 'build');

module.exports = {
  context: SRC_PATH,
  entry: [
    './index.js',
  ],
  output: {
    path: BUILD_PATH,
    filename: 'bundle.js'
  },
  devtool: "source-map",
  module: {
    rules: [{
        test: /\.js$/,
        include: SRC_PATH,
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['env']
          }
        }
      },
      {
        test: /index\.css$/,
        include: SRC_PATH,
        use: [
          {
            loader: MiniCSSExtractPlugin.loader,
          },
          {
            loader: 'css-loader',
          },
         ],
        },
    ]
  },
  plugins: [
    new MiniCSSExtractPlugin({
      filename: 'style.css',
    }),
    new HTMLWebpackPlugin({
      filename: 'index.html',
      template: './index.html'
    })
  ]
};