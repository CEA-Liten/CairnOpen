# -*- coding: utf-8 -*-
"""
Created on Tue Jan 30 11:16:51 2018

@author: NL250993
"""

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.dates as mdates
import subprocess
import numpy as np
import pandas as pd
from pathlib import Path 

""" PLOT DATA """
font_axes='18'
font_ticks='16'
font_legends='16'
font_weight_axes='bold'
grid_alpha=0.4
grid_linewidth=0.3
grid_style='dashed'
lstyle='--'
lw=2
ms='8'
color_p=['r','b','k','g','c','m','y','brown','aqua','darkmagenta','olive',\
'dimgray','tomato','darkblue','forestgreen','goldenrod','mistyrose','palegreen','paleturquoise','orangered',\
'r','b','k','g','c','m','y','brown','aqua','darkmagenta','olive',\
'dimgray','tomato','darkblue','forestgreen','goldenrod','mistyrose','palegreen','paleturquoise','orangered']
marker_p=['v','s','<','o','>','h','D','x','*','v','s','<','o','>','h','D','x','*']
plt.rc('font', weight='bold')
# date_format ='%d/%m'

inkscapePath = r"C:\Program Files\Inkscape\inkscape.exe"

def save_emf(title,fig1):
    svgtitle = str(title).replace('.png','.svg')
    emftitle = str(title).replace('.png','.emf')
    fig1.savefig(svgtitle,bbox_inches='tight')
    subprocess.run([inkscapePath, svgtitle, '-M', emftitle])

    
def plot_parity(df, col_ref, col_comp, file_to_save, 
                variable_label = 'Puissance', unit = ' [MW]', 
                ink = False, color = color_p, marker = marker_p,
                lim = [0,1], islim = False, normed = 1):
    fig1 = plt.figure(figsize=(10,10))
    gs = gridspec.GridSpec(1, 1)
    ax0 = plt.subplot(gs[0])
    i = 0
    for k in col_comp:
        ax0.plot(df[col_ref]/normed, df[k]/normed, color=color[i], ls='None', markersize = 6, marker = marker[i], alpha=0.5)
        i = i+1
    
    plt.xticks(fontsize = font_ticks)
    plt.yticks(fontsize = font_ticks)
    ax0.set_xlabel(r'Ref: ' + variable_label + unit , fontsize= font_axes, fontweight=font_weight_axes)
    ax0.set_ylabel(r'New: ' + variable_label + unit , fontsize= font_axes, fontweight=font_weight_axes)
    if islim:
        ax0.plot(lim, lim, ls='-', color= 'r', alpha=1)
        ax0.plot(lim, [lim[0], 1.2*lim[1]], ls='--', color= 'r', alpha=1)
        ax0.plot(lim, [lim[0], 0.8*lim[1]], ls='--', color= 'r', alpha=1)
        ax0.set_xlim([lim[0], lim[1]])
        ax0.set_ylim([lim[0], lim[1]])
    fig1.savefig(file_to_save,bbox_inches='tight')
    if ink:
        save_emf(file_to_save,fig1)
        

def plot_density(df, col_ref, col_comp, file_to_save, bin_number = 100,
                variable_label = 'Puissance', unit = ' [MW]', 
                ink = False, color = color_p, marker = marker_p[0],
                lim = False, xlim = [0,1], ylim = [0,1], nbins = 20,
                tri_null = False):
    if tri_null:
        df2=df[df[col_ref]>0].copy()
        df2=df2[df2[col_comp]>0]
    else:
        df2 = df.copy()
         
    erreur = df2[col_ref] - df2[col_comp]
    bins=np.linspace(-np.maximum(erreur.max(),-erreur.min()),np.maximum(erreur.max(),-erreur.min()),nbins)
    if lim:
        bins=np.linspace(xlim[0],xlim[1],nbins)
    counts,bin_edges = np.histogram(erreur,bins)
    counts = 100*counts/len(erreur)
    bin_centres = (bin_edges[:-1] + bin_edges[1:])/2.
    
    fig1 = plt.figure(figsize=(10,10))
    gs = gridspec.GridSpec(1, 1)
    ax0 = plt.subplot(gs[0])
    ax0.plot(bin_centres, counts, color='k', marker = marker, markersize=6, lw=2, alpha=1)
    plt.grid(True)
    plt.xticks(fontsize = font_ticks)
    plt.yticks(fontsize = font_ticks)
    ax0.set_xlabel(r'Erreur(Ref-New)  sur ' + variable_label + unit , fontsize= font_axes, fontweight=font_weight_axes)
    ax0.set_ylabel(r'Densité [%]', fontsize= font_axes, fontweight=font_weight_axes)
    if lim:
        ax0.set_xlim(xlim)
        # ax0.set_ylim(ylim)
    fig1.savefig(file_to_save,bbox_inches='tight')
    if ink:
        save_emf(file_to_save,fig1)


def plot_independant_var(data_plot, var_line,title= 'default',
              resam = '1H', ink = False,
              xlabel = r'Var x', ylabel = r'Var y', alpha = 0.7):
    data_plot = data_plot.resample(resam).mean()
    fig1 = plt.figure(figsize=(10,10))
    gs = gridspec.GridSpec(1, 1)
    ax0 = plt.subplot(gs[0])
    size_inc = 0
    for key in var_line.keys():
            ax0.plot(data_plot[var_line[key]['x_entry']], 
                     data_plot[var_line[key]['y_entry']],
                     color=var_line[key]['color'], 
                     ls=var_line[key]['ls'],
                     lw=var_line[key]['lw'],
                     alpha=alpha, 
                     marker = var_line[key]['marker'], 
                     label = var_line[key]['label'],
                     markersize = max(1,7-size_inc))
            size_inc += 1
    plt.xticks(fontsize='16')
    plt.yticks(fontsize='16')
    ax0.legend(loc='best', shadow=True, fontsize='18', ncol=2)
    ax0.set_xlabel(xlabel, fontsize='18', fontweight="bold")
    ax0.set_ylabel(ylabel, fontsize='18', fontweight="bold")
    ax0.tick_params(labelsize='18')    
    fig_title = title+'_' + resam+'.png'
    fig1.savefig(fig_title,bbox_inches='tight')
    if ink:
        save_emf(fig_title,fig1)


def plot_days(data_plot, var_fill, var_line, title= 'default',
              resam = '24H', ink = False, lim_dates = None, 
              monotone = [True, 'Demande'], date_format = '%d/%m',
              xlabel = r'Temps [Jour]', ylabel = r'Puissance thermique [$\mathbf{MW_{th}}$]', alpha = 0.7, var_line_plot_y2 = None ,y2label = None,ylim = None, dossier =""):

    if lim_dates != None:
        data_plot = data_plot[data_plot.index>lim_dates[0]]
        data_plot = data_plot[data_plot.index<lim_dates[1]]
    data_plot = data_plot.resample(resam).mean()
    fig1 = plt.figure(figsize=(15,8))
    gs = gridspec.GridSpec(1, 1)
    ax0 = plt.subplot(gs[0])
    if var_line != None:
        for key in var_line.keys():
            ax0.plot(data_plot.index, data_plot[key], color=var_line[key]['color'], lw=1,alpha=1, marker = var_line[key]['marker'], label = var_line[key]['label'])
    if var_fill != None:
        for i in range (1,len(var_fill.keys())+1):
            for m in var_fill.keys():
                if i == var_fill[m]['order']:
                    if i == 1:
                        ax0.fill_between(data_plot.index, 0, data_plot[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                        base = data_plot[m]
                    else:
                        ax0.fill_between(data_plot.index, base, base + data_plot[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                        base += data_plot[m]
                if i == -var_fill[m]['order']:
                    if i == 1:
                        ax0.fill_between(data_plot.index, 0, -data_plot[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                        base_neg = -data_plot[m]
                    else:
                        ax0.fill_between(data_plot.index, -base_neg, -base_neg - data_plot[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                        base_neg -= data_plot[m]
    if var_line_plot_y2 != None:
        ax2 = ax0.twinx()  # instantiate a second axes that shares the same x-axis
        for key in var_line_plot_y2.keys():
            ax2.plot(data_plot.index, data_plot[key], color=var_line_plot_y2[key]['color'], lw=1,alpha=1, marker = var_line_plot_y2[key]['marker'], label = var_line_plot_y2[key]['label'])
            ax2.set_ylabel(y2label, color=var_line_plot_y2[key]['color'], fontsize='18')  # we already handled the x-label with ax1
            ax2.tick_params(axis='y', labelcolor=var_line_plot_y2[key]['color'])                
    plt.xticks(fontsize='16')
    plt.yticks(fontsize='16')
    ax0.legend(loc='upper center', shadow=True, fontsize='18', ncol=2,bbox_to_anchor=(0.5, -0.2))
    ax0.set_xlabel(xlabel, fontsize='18', fontweight="bold")
    ax0.set_ylabel(ylabel, fontsize='18', fontweight="bold")
    ax0.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
    ax0.tick_params(labelsize='18')    
#    if lim_dates != None:
#        ax0.set_xlim(lim_dates)
    if ylim != None:
        ax0.set_ylim(ylim)
    plt.xticks(rotation=70)
    plt.title(title)
    fig_title = title+resam+'.png'
    fig1.savefig(dossier+fig_title,bbox_inches='tight')
    if ink:
        save_emf(dossier+fig_title,fig1)
    
    if monotone[0]:
        data_plot_sort = data_plot.sort_values(monotone[1],ascending=False)
        data_plot_sort['Days'] = np.arange(data_plot_sort.shape[0])
        fig1 = plt.figure(figsize=(15,8))
        gs = gridspec.GridSpec(1, 1)
        ax0 = plt.subplot(gs[0])
        if var_line != None:
            for key in var_line.keys():
                ax0.plot(data_plot_sort['Days'], data_plot_sort[key], color=var_line[key]['color'], lw=1,alpha=1, marker = var_line[key]['marker'], label = var_line[key]['label'])
        if var_fill != None:
            for i in range (1,len(var_fill.keys())+1):
                for m in var_fill.keys():
                    if i == var_fill[m]['order']:
                        if i == 2:
                            ax0.fill_between(data_plot_sort['Days'], 0, data_plot_sort[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                            base = data_plot_sort[m]
                        elif i !=1:
                            ax0.fill_between(data_plot_sort['Days'], base, base + data_plot_sort[m], color=var_fill[m]['color'], lw=0, label=var_fill[m]['label'],alpha=alpha)
                            base += data_plot_sort[m]
        plt.xticks(fontsize='16')
        plt.yticks(fontsize='16')
        ax0.set_xlim([0, data_plot_sort.shape[0]]) 
        ax0.legend(loc='upper center', shadow=True, fontsize='18', ncol=2,bbox_to_anchor=(0.5, -0.2))
        ax0.set_xlabel(xlabel, fontsize='18', fontweight="bold")
        ax0.set_ylabel(ylabel, fontsize='18', fontweight="bold")
        fig_title = title+resam+'_monotone.png'
        fig1.savefig(dossier+fig_title,bbox_inches='tight')
        if ink:
            save_emf(dossier+fig_title,fig1)
            
            
def plot_months(data_plot, var, title= 'default', 
              ink = False, width = 0.8, ylim = None,
              ylabel = r'Energie [$\mathbf{GWh_{th}}$]',ratio_to_unit=0.001, alpha = 0.8):

    dfmonth = pd.DataFrame()
    for k in var:
        dfmonth[k] = [data_plot[data_plot['Months']==i][k].sum() for i in range(1,13)]
    dfmonth['month'] = ['Jan', 'Fev', 'Mar', 'Avr', 'Mai', 'Juin', 'Juil', 'Aout', 'Sep', 'Oct', 'Nov', 'Dec']
    dfmonth = dfmonth.set_index('month')

    fig1 = plt.figure(figsize=(15,8))
    gs = gridspec.GridSpec(1, 1)
    ax0 = plt.subplot(gs[0])
    if var != None:
        for i in range (1,len(var.keys())+1):
            for m in var.keys():
                if i == var[m]['order']:
                    if i == 1:
                        ax0.bar(dfmonth.index, dfmonth[m]*ratio_to_unit, width = width, edgecolor ='k', color=var[m]['color'],alpha=alpha, label = var[m]['label'])
                        bottom = dfmonth[m]*ratio_to_unit
                    else:
                        ax0.bar(dfmonth.index, dfmonth[m]*ratio_to_unit, width = width, bottom= bottom,edgecolor ='k', color=var[m]['color'],alpha=alpha, label = var[m]['label'])
                        bottom += dfmonth[m]*ratio_to_unit
    plt.xticks(fontsize='16')
    plt.yticks(fontsize='16')
    ax0.legend(loc='best', shadow=True, fontsize='18', ncol=2)
    ax0.set_ylabel(ylabel, fontsize='18', fontweight='bold')
    if ylim != None:
        ax0.set_ylim(ylim) 
    ax0.tick_params(labelsize='18')    
    plt.xticks(rotation=50, fontweight='bold')        
    fig1.savefig(title,bbox_inches='tight')
    if ink:
        save_emf(title,fig1)



def convert(day): 
    import time
    return time.strftime("%d/%m", time.gmtime(day*24*3600))


def carpet_plots(data_plot, var, var_ratio, ink=False,cmap_in = 'RdBu_r', title= 'default', var_ratio_use = True,vmin_cmap = 0, vmax_cmap = 100, min_max_use= False):

    ## Create Plots
    cmap = plt.get_cmap(cmap_in)
    ygrid, xgrid = np.mgrid[slice(1, 366, 1),slice(0, 24*3600/3600, 3600/3600)]
    if var_ratio_use:
        variables_to_plot = list(var.keys()) + [var_ratio]
    else:
        variables_to_plot = list(var.keys())
    carpet_var = {}
    
    
    for k in variables_to_plot:
        carpet_var[k] = []
        for l in range(1,366):
            new_vect = data_plot[data_plot['Days'] == l][k]
            if len(new_vect) < 24:
                new_vect = pd.Series(0, index=range(1,25))
            carpet_var[k].append(new_vect)
        carpet_var[k] = np.vstack(carpet_var[k])
        carpet_var[k] = carpet_var[k][:-1, :-1] 
    
    
    for k in range(0,len(variables_to_plot)):
        fig1 = plt.figure(figsize=(10,10))
        gs = gridspec.GridSpec(1, 1)
        
        if min_max_use:
            vmin_cmap = var[variables_to_plot[k]]['min']
            vmax_cmap = var[variables_to_plot[k]]['max']
        
        ax0 = plt.subplot(gs[0])
        if var_ratio_use:
            im2 = ax0.pcolormesh(xgrid, ygrid, 100*carpet_var[variables_to_plot[k]]/carpet_var[variables_to_plot[-1]], vmin=vmin_cmap, vmax=vmax_cmap, cmap=cmap)
        else: 
            im2 = ax0.pcolormesh(xgrid, ygrid, carpet_var[variables_to_plot[k]], vmin=vmin_cmap, vmax=vmax_cmap, cmap=cmap)
        plt.xticks(np.arange(0, 24, step=4),fontsize='14')
        plt.yticks(fontsize='14')
        ticks_y = np.arange(0,365,60)
        ticks_y_year = [convert(day) for day in ticks_y]
        ax0.set_yticks(ticks_y)
        ax0.set_yticklabels(ticks_y_year)
        ax0.set_xlabel(r'Heures [$\mathbf{h}$]', fontsize='18', fontweight="bold")
        ax0.set_ylabel(r'Jours [$\mathbf{-}$]', fontsize='18', fontweight="bold")
        if var_ratio_use:
            ax0.set_title(variables_to_plot[k] + ' [%]', fontsize='18', fontweight="bold")
        else:
            ax0.set_title(variables_to_plot[k] + ' ' + var[variables_to_plot[k]]['unit'], fontsize='18', fontweight="bold")
        cbar = fig1.colorbar(im2, ax=ax0)
        cbar.ax.get_yaxis().labelpad = 15
        if var_ratio_use:
            cbar.ax.set_ylabel('Part dans le mix [%]', rotation=270, fontsize='18', fontweight="bold")
        cbar.ax.tick_params(labelsize='16')
        
        fig_title = title+ variables_to_plot[k] + '.png'
        fig1.savefig(fig_title,bbox_inches='tight')
        if ink:
            save_emf(fig_title,fig1)

