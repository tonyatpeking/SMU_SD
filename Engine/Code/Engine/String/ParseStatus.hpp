﻿#pragma once


enum ParseStatus
{
    PARSE_SUCCESS,
    PARSE_EMPTY,
    PARSE_INVALID_TOKEN,
    PARSE_TOO_MANY_TOKENS,
	PARSE_TOO_FEW_TOKENS,
    PARSE_WRONG_NUMBER_OF_TOKENS,
    PARSE_INVALID_INPUT,
	PARSE_UNKNOW_ERROR
};