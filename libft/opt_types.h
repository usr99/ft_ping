/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opt_types.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 00:38:13 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/19 12:27:54 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _OPT_TYPES_H_
# define _OPT_TYPES_H_

# include <stdint.h>

typedef uint8_t bool;
enum e_boolval
{
	false,
	true
};

/* Value types expected in a program/option parameter */
typedef enum e_paramtype
{
	PARAM_T_INT8, // not implemented yet
	PARAM_T_INT16, // not implemented yet
	PARAM_T_INT32,
	PARAM_T_INT64, // not implemented yet
	PARAM_T_FLOAT32, // not implemented yet
	PARAM_T_STRING
} t_paramtype;

/* Types of arguments returned */
typedef enum e_argtype
{
	ARG_T_OPTION,
	ARG_T_PARAMETER,
	ARG_T_ERROR,
} t_argtype;

/* Option parser error codes */
typedef enum e_errtype
{
	ERR_BAD_OPTION,
	ERR_MISSING_PARAM,
	ERR_BAD_PARAM_TYPE
} t_errtype;

/* Structure returned when argtype = ARG_T_OPTION */
struct s_optinfo
{
	char name;
	void* value;
};

/* Structure returned when argtype = ARG_T_PARAMETER */
struct s_paraminfo
{
	void* value;
};

/* Structure returned when argtype = ARG_T_ERROR */
struct s_errinfo
{
	char name;
	t_errtype type;
};

/* Describe expected options in arguments */
typedef struct s_expected_opts
{
	char name;
	bool has_param;
	t_paramtype paramtype;
} t_expected_opts;

/* Describe an argument returned after parsing */
typedef struct s_argument
{
	t_argtype type;
	union
	{
		struct s_optinfo opt;
		struct s_paraminfo param;
		struct s_errinfo err;
	} info;
} t_argument;

#endif
