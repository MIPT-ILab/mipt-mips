const path = require('path');

const HTMLWebpackPlugin = require('html-webpack-plugin');
const MiniCSSExtractPlugin = require('mini-css-extract-plugin');
const webpack = require('webpack');
const SRC_PATH = path.resolve(__dirname, 'src');
const BUILD_PATH = path.resolve(__dirname, 'build');

module.exports = {
  context: SRC_PATH,
  entry: {
    main: './index.js',
  },
  output: {
    path: BUILD_PATH,
    filename: 'bundle.js'
  },
  devtool: "source-map",
  module: {
    rules: [
      {
        test: /\.js$/,
        include: SRC_PATH,
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['@babel/env']
          }
        }
      },
      {
        test: /\.css$/,
        include: SRC_PATH,
        use: [
          { loader: MiniCSSExtractPlugin.loader },
          { loader: 'css-loader' },
        ]
      },
      {
        test: /shadow\.css$/,
        include: SRC_PATH,
        use: [
          { loader: 'css-loader' },
        ]
      }
    ]
  },
  plugins: [
    new MiniCSSExtractPlugin({
      filename: 'css/index.css'
    }),
    new HTMLWebpackPlugin({
      hash: 'true',
      inject: 'false',
      template: './index.html',
      filename: 'index.html',
      chunks: ['main']
    })
  ]
};