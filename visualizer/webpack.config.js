// Copyright 2019 MIPT-MIPS
/*
 * (Type docs)
 * @author Eric Konks konks.em@phystech.edu
*/

const path = require('path');
const fs = require('fs')
const HTMLWebpackPlugin = require('html-webpack-plugin');
const MiniCSSExtractPlugin = require('mini-css-extract-plugin');
const CopyPlugin = require('copy-webpack-plugin');
const webpack = require('webpack');
const SRC_PATH = path.resolve(__dirname, 'src');
const BUILD_PATH = path.resolve(__dirname, 'build');

module.exports = env => {
  try {
    fs.mkdirSync(BUILD_PATH);
  } catch (EEXIST) {
    // continue regardless of error
  }
  if (!fs.existsSync(env.path)) {
    const err = `Couldn't open file ${env.path}`
    throw(new Error(err));
  }
  return { 
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
        },
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
      }),
      
      new webpack.DefinePlugin({
         'process.env': {
          'path': JSON.stringify(path.resolve(BUILD_PATH, 'topology.json')),
        }
      }),
      new CopyPlugin([
        { from : path.resolve(env.path), to: BUILD_PATH }
      ])
    ]
  }
};
