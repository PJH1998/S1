// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogS1, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogS1Warning, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogS1Error, Error, All);
DECLARE_LOG_CATEGORY_EXTERN(LogS1Fatal, Fatal, All);


#define LOG(Format, ...) UE_LOG(LogS1, Log, Format, ##__VA_ARGS__)
#define LOG_WARNING(Format, ...) UE_LOG(LogS1Warning, Warning, Format, ##__VA_ARGS__)
#define LOG_ERROR(Format, ...) UE_LOG(LogS1Error, Error, Format, ##__VA_ARGS__)
#define LOG_FATAL(Format, ...) UE_LOG(LogS1Fatal, Fatal, Format, ##__VA_ARGS__)